/**

Program: Java FX Application

Filename: EditBillForm.java

@author: © Charalampos Nikolaidis 

Course: BSc Computing

Module: Visual Object Software

Tutor: Theodoros Manavis

@version: 3.1

Date: 14/07/23

*/

import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.io.*;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class EditBillForm extends Application {

    private Billing billing; // The billing instance
    private TextField dateTextField; // Text field for the date
    private TextField billNumberTextField; // Text field for the bill number
    private Functions functions; // Helper class instance

    public EditBillForm(Billing billing) {
        this.billing = billing;
        functions = new Functions(); // Create an instance of the helper class
    }

    public static void main(String[] args) {
        launch(args); // Launch the application
    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Edit Bill Form"); // Set the title of the stage

        VBox vBox = new VBox(); // Create a vertical box layout
        vBox.setSpacing(10); // Set spacing between elements
        vBox.setPadding(new Insets(10)); // Set padding around the vertical box

        Label dateLabel = new Label("Date:"); // Create a label for the date
        dateTextField = new TextField(billing.getMonth()); // Create a text field with the billing's month pre-filled

        Label billNumberLabel = new Label("Bill Number:"); // Create a label for the bill number
        billNumberTextField = new TextField(String.valueOf(billing.getAmount())); // Create a text field pre-filled with the bill amount

        Button submitButton = new Button("Submit"); // Create a submit button
        submitButton.setOnAction(e -> {
            String newDate = dateTextField.getText(); // Get the entered date
            String newAmountStr = billNumberTextField.getText(); // Get the entered amount

            if (newDate.isEmpty() || newAmountStr.isEmpty()) {
                functions.showMessage("Please enter all fields.", false, true); // Show an error message if any field is empty
                return; // Stop further execution if any field is empty
            }

            if (!functions.isNumber(newAmountStr)) { // Check if the entered amount is a valid number
                functions.showMessage("Please enter a valid number for the amount.", false, true); // Show an error message if the amount is not valid
                return; // Stop further execution if the amount is not valid
            }

            double newAmount = Double.parseDouble(newAmountStr); // Convert the entered amount to a double
            saveChanges(newDate, newAmount); // Save the changes to the billing record
            functions.showMessage("Successfully edited bill!", false, true); // Show a success message
        });

        vBox.getChildren().addAll(dateLabel, dateTextField, billNumberLabel, billNumberTextField, submitButton); // Add UI elements to the vertical box

        Scene scene = new Scene(vBox, 400, 200); // Create a scene with the vertical box layout
        primaryStage.setScene(scene); // Set the scene on the stage

        primaryStage.show(); // Display the stage
    }

    private void saveChanges(String newDate, double newAmount) {
        String updatedData = ""; // String to store the updated data
        List<String> matchedLines = new ArrayList<>(); // List to store lines with duplicated months
        double oldTotalAmount = 0;

        try (BufferedReader reader = new BufferedReader(new FileReader("billing.csv"))) { // Read data from the "billing.csv" file
            String line;
            boolean isFirstLine = true;
            while ((line = reader.readLine()) != null) {
                if (isFirstLine) {
                    updatedData += line + System.lineSeparator(); // Preserve the headers
                    isFirstLine = false;
                    continue;
                }
                String[] parts = line.split(",");
                int customerId = Integer.parseInt(parts[0]);
                String month = parts[1];
                double amount = Double.parseDouble(parts[2]);
                DecimalFormat decimalFormat = new DecimalFormat("#.##");
                amount = Double.parseDouble(decimalFormat.format(amount));
                
                if (customerId == billing.getCustomerId() && month.equals(billing.getMonth())) { // Check if the line matches the billing's customer ID and month
                    matchedLines.add(line); // Add the line to the list of matched lines
                    oldTotalAmount += amount; // Add the amount to the total amount
                } else {
                    updatedData += line + System.lineSeparator(); // Preserve other records
                }
            }
        } catch (IOException e) {
            System.out.println("Error reading data: " + e.getMessage()); // Print an error message if there's an error reading data
            return;
        }

        // Calculate the new amount for each duplicated month
        double newDistributedAmount = (oldTotalAmount - billing.getAmount() + newAmount) / matchedLines.size();
        for (String line : matchedLines) {
            String[] parts = line.split(",");
            int customerId = Integer.parseInt(parts[0]);
            String updatedLine = customerId + "," + newDate + "," + newDistributedAmount;
            updatedData += updatedLine + System.lineSeparator();
        }

        try (BufferedWriter writer = new BufferedWriter(new FileWriter("billing.csv"))) { // Write data to the "billing.csv" file
            writer.write(updatedData); // Write the updated data to the file
            writer.flush(); // Flush the writer
            System.out.println("Changes saved successfully!"); // Print a success message
        } catch (IOException e) {
            System.out.println("Error saving changes: " + e.getMessage()); // Print an error message if there's an error saving changes
        }
    }
}
