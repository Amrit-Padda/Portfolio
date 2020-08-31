public class FinalProject {
	/* Constants */
	// Masses in kg
	final private static double DriverMass = 80;
	final private static double CarMass = 1510;
	final private static double TMass = DriverMass + CarMass;
	// Ratios
	final private static double GearR = 3.769;												// Final Drive Ratio
	final private static double[] TransR = {3.827, 2.360, 1.685, 1.312, 1.000, 0.793};		// Gear Ratios for gears 1-6
	final private static double[] TransEff = {0.634, 0.676, 0.716, 0.773, 0.829, 0.853};	// Transmission Efficiency at each gear
	// Wheel
	final private static double RWheel = 0.3175;	// Radius of the wheel in m
	final private static double TirePress = 2.21;	// Tire pressure in Bar
	// Air
	final private static double AirDense = 1.225;	// Air Density in kg/m^3
	final private static double AirVel = 0;			// Positive in the negative X direction (opposite motion of the vehicle)
	// Other
	final private static double CdA = 6.44;			// Automobile Drag Coefficient 
	final private static double Grav = 9.8;			// in m/s^2
	final private static double dt = 0.001;
	final private static double MuK = 0.8;			// Coefficient of static friction (rubber/concrete)
	final private static double tol = 0.01;
	// Initial Speed
	private static double iniSpeed = speedConverter(5,1);	// km/h /3.6 = m/s
	// Final Speed
	private static double finSpeed = speedConverter(100,1);	// km/h /3.6 = m/s
	// Gear Switch range
	private static double minAngVel = 0;
	private static double maxAngVel = 670.2;		// (Redline)Found using powercurve in rad/s


	public static void main (String[] args){
		// Points on the power curve
		double angVelA = minAngVel, angVelB, angVelC = maxAngVel, angVelD=0;
		double timeB =0, timeD = 0;

		angVelB = 0.382 * (angVelC-angVelA);
		angVelD = angVelC+angVelA-angVelB;
		// The bisection method to optimise the angular velocity at which to shift gears
		while (angVelC-angVelA > tol) {
			if (angVelC-angVelB>angVelB-angVelA) {
				angVelD = angVelB + 0.382*(angVelC-angVelB);
				timeB = timeCalc(angVelB);
				timeD = timeCalc(angVelD);
				if (timeB > timeD) {
					angVelA = angVelB;
					angVelB = angVelD;
				} else
					angVelC= angVelD;
			} else {
				angVelD = angVelB - 0.382*(angVelB - angVelA);
				timeB = timeCalc(angVelB);
				timeD = timeCalc(angVelD);
				if (timeB > timeD) {
					angVelC = angVelB;
					angVelB = angVelD;
				} else
					angVelA = angVelD;
			}
		}

		// Result display
		System.out.printf("The best time to switch gears is at an engine RPM of %.2f\n", angVelD*9.55);
		System.out.println(+Math.round((angVelD/maxAngVel)*10000)/100.0+"% of the red line");
		System.out.printf("The time it took to go from %.1fkm/h to %.1fkm/h is: %.3f\n", speedConverter(iniSpeed, 2), speedConverter(finSpeed, 2), timeD);

	}
	// Calculates the torque from an angular velocity using the power curve function
	public static double torqueCalc (double angVel) {
		double torque;
		torque = -0.002*Math.pow(angVel, 2)+1.854*(angVel);
		return torque;
	}
	// Calculates the traction force from a known torque
	public static double tractForceCalc (double torque, int gear) {
		double tForce;

		tForce = (torque*GearR*TransR[gear]*TransEff[gear])/RWheel;

		return tForce;
	}
	// Calculates the drag force based on speed of the vehicle
	public static double dragForceCalc (double speed){
		double dForce;

		dForce = 0.5*CdA*AirDense*(speed+AirVel);

		return dForce;
	}
	// Calculates the friction force between the tire and the surface
	public static double frictForceCalc (double speed){
		double fForce, c;

		c = 0.005 +(0.01+0.0095*Math.pow((speed/100),2))/TirePress;
		fForce = c * TMass * Grav;

		return fForce;
	}
	// Converts speed from m/s to km/h
	public static double speedConverter (double speed, int decider) {
		if (decider == 1)
			speed = speed/3.6; // Converts to m/s
		else
			speed = speed*3.6; // Converts to km/h
		return speed;
	}
	// The method that simulates the motion of the car and returns the time
	public static double timeCalc (double angVelSwitch) {
		int gear = 0;
		double speed, angVel, torque, accel, time =0;

		speed = iniSpeed;
		angVel = speed/RWheel*GearR*TransR[gear]*TransEff[gear];
		torque = torqueCalc(angVel);

		while (speed < finSpeed) {
			accel = (tractForceCalc(torque, gear) - dragForceCalc(speed) - frictForceCalc(speed))/TMass;
			speed = speed + accel*dt;
			time = time + dt;
			angVel = speed/RWheel*GearR*TransR[gear]*TransEff[gear];
			if (angVel > angVelSwitch && gear < 5) {
				gear++;
				angVel = speed/RWheel*GearR*TransR[gear]*TransEff[gear];
			}
			torque = torqueCalc(angVel);
		}
		return time;
	}
}
