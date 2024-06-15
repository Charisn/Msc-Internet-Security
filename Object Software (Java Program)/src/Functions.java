/**

Program: Java FX Application

Filename: Functions.java

@author: © Charalampos Nikolaidis 

Course: BSc Computing

Module: Visual Object Software

Tutor: Theodoros Manavis

@version: 3.1

Date: 14/07/23

*/

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

public class Functions {

    public ObservableList<Customer> readCustomersFromCSV() {
        ObservableList<Customer> customers = FXCollections.observableArrayList();
        File csvFile = new File("customers.csv");
    
        try {
            boolean fileExists = csvFile.exists();

        if (!fileExists) {
            FileWriter writer = new FileWriter(csvFile);
            writer.write("CustomerID,Name,PhoneNumber,Address,EnergyBilling,MeterType\n"); // Write headers
            writer.close();
        }
    
            BufferedReader reader = new BufferedReader(new FileReader(csvFile));
            String line;
            reader.readLine(); // Skip header line
            while ((line = reader.readLine()) != null) {
                String[] data = line.split(",");
                if (data.length == 6) { // Check if the data length is correct
                    int id = Integer.parseInt(data[0]);
                    String name = data[1];
                    String phoneNumber = data[2];
                    String address = data[3];
                    String billing = data[4];
                    String meterType = data[5];
                    Customer customer = new Customer(id, name, phoneNumber, address, billing, meterType);
                    System.out.println("Read customer: " + customer); // Debug output
                    customers.add(customer);
                } else {
                    System.err.println("Incorrect data format in line: " + line); // Debug output
                }
            }
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    
        System.out.println("Read " + customers.size() + " customers from CSV"); // Debug output
    
        return customers;
    }

    public boolean isNumber(String str) {
        return str.matches("-?\\d+(\\.\\d+)?");
    }

    public GridPane createForm(int gap1, int gap2) {
        GridPane gridPane = new GridPane();
        gridPane.setPadding(new Insets(gap1));
        gridPane.setHgap(gap1);
        gridPane.setVgap(gap2);

        return gridPane;
    }

    public void showMessage(String str, boolean cancel, boolean success) {
        Stage successStage = new Stage();
        if (success) {
            successStage.setTitle("Success");
        } else {
            successStage.setTitle("Error");
        }

        Label successLabel = new Label(str); // Create a label for the success message
        Button okButton = new Button("Confirm"); // Create an OK button
        okButton.setOnAction(e -> {
            successStage.close(); // Close the success stage
        });

        if (cancel) {
            Button cancelButton = new Button("Cancel"); // Create a Cancel button
            cancelButton.setOnAction(e -> {
                successStage.close(); // Close the success stage
                return;
            });

            VBox successBox = new VBox(successLabel, okButton, cancelButton); // Create a VBox with label, OK button, and Cancel button
            successBox.setAlignment(Pos.CENTER); // Center align the content within the VBox
            successBox.setPadding(new Insets(20)); // Set padding around the VBox
            successBox.setSpacing(10); // Set spacing between elements

            // Calculate the preferred width of the VBox based on the message length
            double preferredWidth = successLabel.getFont().getSize() * str.length();

            Scene successScene = new Scene(successBox, preferredWidth, 100); // Create a scene with the dynamic width
            successStage.setScene(successScene); // Set the scene on the success stage
            successStage.showAndWait(); // Display the success stage
        } else {
            VBox successBox = new VBox(successLabel, okButton); // Create a VBox with label and OK button
            successBox.setAlignment(Pos.CENTER); // Center align the content within the VBox
            successBox.setPadding(new Insets(20)); // Set padding around the VBox
            successBox.setSpacing(10); // Set spacing between elements

            // Calculate the preferred width of the VBox based on the message length
            double preferredWidth = successLabel.getFont().getSize() * str.length();

            Scene successScene = new Scene(successBox, preferredWidth, 100); // Create a scene with the dynamic width
            successStage.setScene(successScene); // Set the scene on the success stage
            successStage.showAndWait(); // Display the success stage
        }
    }
}
