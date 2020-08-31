package ca.concordia.comp_445.utils;

public class Pair<First, Second> {
    public static <P, Q> Pair<P, Q> make(P first, Q second) {
        return new Pair<P, Q>(first, second);
    }

    public First first;
    public Second second;

    public Pair(First first, Second second) {
        this.first = first;
        this.second = second;
    }
}
