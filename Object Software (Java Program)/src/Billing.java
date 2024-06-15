/**

Program: Java FX Application

Filename: Billing.java

@author: © Charalampos Nikolaidis 

Course: BSc Computing

Module: Visual Object Software

Tutor: Theodoros Manavis

@version: 3.1

Date: 14/07/23

*/

public class Billing {
    private int customerId; // Customer ID associated with billing
    private String month; // Month of billing
    private double amount; // Amount of billing

    public Billing(int customerId, String month, double amount) {
        this.customerId = customerId; // customer ID
        this.month = month; // month
        this.amount = amount; // amount
    }

    public int getCustomerId() {
        return customerId; // Return customer ID
    }

    public String getMonth() {
        return month; // Return month
    }

    public double getAmount() {
        return amount; // Return amount
    }

    public void setMonth(String month) {
        this.month = month; // Set month
    }

    public void setAmount(double amount) {
        this.amount = amount; // Set amount
    }

    @Override
    public String toString() {
        return "\nAccount ID: " + customerId + // Return billing information as a formatted string
                "\nMonth: " + month +
                "\nAmount: $" + amount;
    }
}
