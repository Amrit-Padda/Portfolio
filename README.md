# Portfolio
Here you will find some of my current projects.


## Doc Processor
One of the major initiatives I've led is to onboard an electronic EMR (Electronic Medical Records) system for a small medical clinic. After setting up and configuring the server, we began the process of scanning paper files and uploading them into the system. 
In order to speed up the process for the staff, I built a python script which uses Google's OCR API to read the scanned document, and sort it into the appropriate directory. 
The biggest challenge here was finding unique text on each document to sort by.

## SAE Data Acquisition System
This ongoing project is to develop a data acquisition system for our offroad buggy. It is written in the Arduino language. 
A few interesting constraints and concerns with the project are as follows: 
- The data from the sensors must be read and saved to an IO device with as little latency as possible, hence the system must be        "real-time". This was achieved by making individual "slave" devices which recieve data from sensors and forward them onto a CANBUS.   A singular "master" device will then listen on the bus for data to arrive and complete the required IO operations. This ensures that the "slave" is not slowed down by the IO process and can transmit data immediately. 
- The system needs to be powered by the engine's alternator, this required me to design a power delivery system which provides a constant voltage to the various devices connected to the system. The engine uses a motorcycle style Stator to convert the rotational motion of the engine to an electrical current. This system leads to the current being generated to resemble a sinusoidal wave, which is not usable by the Arduinos. To solve this, a recifier/regulator is installed between the alternator and the battery. This takes the alternating current coming from the alternator to be converted to a smoother DC voltage. Buck converters were also used to further ensure that the power being delivered to each device is constant and the correct voltage. Arduinos are fairly resistant to power fluctuations, however it is considered a best practice to use an external power regulator. 
- The system must also be embedded and operate in harsh conditions, to solve this problem I designed a 3D printed case to house the various devices
  
In the future, the scope of the system will expand to include a suspension sensor package as well as live driver feedback. The focus of the project at the moment is to get engine and gearbox data for our Electronic Transmission Captsone project which is currently under development. 

## Computer Vision 
The Computer Vision directory contains an image stitcher completed for a class on computer vision. The task was completed using Python and the OpenCV library. It proved to be a very challenging course and project, mostly due to the restrictions on
the use of built in methods. Most of the required components had to be implemented from first principles, making it more challenging than most final projects,
but I was able to complete the project with a few small bugs. 

## Artificial Intelligence 
The Artificial Intelligence directory contains a few projects completed for a class on artificial intelligence. The two contained projects are an A Star graph transversal of downtown montreal and a program which classifies the title of a forum post based on a pre built model.
Both very interesting projects, however the A Star implementation was by far my favorite as you could see the algo get better and better at pathfinding as the parameters were tweaked and bugs were fixed. 

## Computer Networking 
The Computer Networking directory contains a project completed for a class on Computer Networking. Here, me and my teammate built a client and server from scratch in Java with both TCP and UDP functionality. This was an extremely eye opening project as these two protocols are how the current internet functions. 
Building it from scratch really gave me an appreciation for how complex the internet really is. I will add that completing this project with Java was not a very fun experience...

## BitKye 
BitKye is a web application I developed which incorporates some blockchain technology, but mostly web development. The fundamental concept is that of a rotating savings and credit association, a fairly simple peer to peer lending schema.
The application allows users to contribute to a pool and then recieve a payout when the pool satisfies a criteria.
When users send a transaction to the BitKye address, an external API triggers a callback url which adds the transaction to a
MySQL database hosted with the AWS RDS service. Eventually, once the pool is full the amount is sent back to the users according to predefined rules. 


## Risk 
Risk is an implementation of the board game risk in c++. My team and I worked to develop a working implementation of Risk in c++ as part of our Advanced Programming class.



  
