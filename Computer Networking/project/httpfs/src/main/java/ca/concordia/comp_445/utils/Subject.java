package ca.concordia.comp_445.utils;

import java.util.ArrayList;
import java.util.List;

public class Subject {
    private List<Observer> observers = new ArrayList<>();

    /**
     * Attach an {@link Observer} that will listen to a {@link Subject}
     *
     * @param observer The observer to add.
     */
    public void attach(Observer observer) {
        this.observers.add(observer);
    }

    /**
     * Detach an {@link Observer} from the {@link Subject} instance.
     *
     * @param observer
     */
    public void detach(Observer observer) {
        this.observers.remove(observer);
    }

    /**
     * Notify the {@link Observer} instances listening to the subject
     * about a certain {@link Event}
     *
     * @param e The event that happened
     */
    public void notify(Event e) {
        this.observers.forEach(observers -> observers.onNotify(e));
    }
}
