/**

Program: Java FX Application

Filename: Customer.java

@author: © Charalampos Nikolaidis 

Course: BSc Computing

Module: Visual Object Software

Tutor: Theodoros Manavis

@version: 3.1

Date: 14/07/23

*/

public class Customer {
    private String name; // Name of  customer
    private String phoneNumber; // Phone number of  customer
    private String address; // Address of  customer
    private String energyBilling; // Energy billing information of  customer
    private String meterType; // Type of meter used by  customer
    private int customerId; // Unique ID of  customer

    public Customer(int customerId, String name, String phoneNumber, String address, String billing, String meterType) {
        this.customerId = customerId; // customer ID
        this.name = name; // customer name
        this.phoneNumber = phoneNumber; //  customer phone number
        this.address = address; //  customer address
        this.energyBilling = billing; // energy billing information
        this.meterType = meterType; // meter type
    }

    public Customer(int customerId, String name) {
        this.customerId = customerId; // customer ID
        this.name = name; //  customer name
    }

    public int getCustomerId() {
        return customerId; // Return customer ID
    }

    public String getName() {
        return name; // Return customer name
    }

    public String getPhoneNumber() {
        return phoneNumber; // Return customer phone number
    }

    public String getAddress() {
        return address; // Return customer address
    }

    public String getEnergyBilling() {
        return energyBilling; // Return energy billing information
    }

    public String getMeterType() {
        return meterType; // Return meter type
    }

    @Override
    public String toString() {
        return "Name: " + name + // Return customer information as a formatted string
            "\nAccount Number: " + customerId +
            "\nPhone Number: " + phoneNumber +
            "\nAddress: " + address +
            "\nEnergy Billing: " + energyBilling +
            "\nMeter Type: " + meterType;
    }
}
