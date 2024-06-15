/**

Program: Java FX Application

Filename: AddBillForm.java

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
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.text.DecimalFormat;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Optional;

public class AddBillForm extends Application {

    public TextField accountNumberField;
    private TextField monthField;
    private TextField amountField;
    private Customer customer;
    private Functions functions;

    public AddBillForm(Customer customer) {
        this.customer = customer;
        functions = new Functions();
    }

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Add Bill: " + customer.getName());

        GridPane gridPane = createFormPane(); // Create the main layout pane
        addUIControls(gridPane); // Add UI controls to the layout pane

        Scene scene = new Scene(gridPane, 400, 200); // Create a scene with the layout pane
        primaryStage.setScene(scene); // Set the scene on the stage

        primaryStage.show(); // Display the stage
    }

    private GridPane createFormPane() {
        GridPane gridPane = new GridPane(); // Create a new GridPane
        gridPane.setPadding(new Insets(20)); // Set padding around the grid pane
        gridPane.setHgap(10); // Set horizontal gap between grid elements
        gridPane.setVgap(10); // Set vertical gap between grid elements

        return gridPane; // Return the created GridPane
    }

    private void addUIControls(GridPane gridPane) {
        int rowIndex = 0;

        // Account Number Field
        accountNumberField = new TextField(); // Create a text field for account number
        gridPane.add(accountNumberField, 1, rowIndex); // Add the text field to the grid pane at (1, rowIndex)
        accountNumberField.setEditable(false); // Set the text field as non-editable
        accountNumberField.setStyle("-fx-background-color: transparent;"); // Set transparent background

        rowIndex++;

        // Month Label
        Label monthLabel = new Label("Month of Charge:"); // Create a label for month
        gridPane.add(monthLabel, 0, rowIndex); // Add the label to the grid pane at (0, rowIndex)

        // Month Field
        monthField = new TextField(); // Create a text field for month
        monthField.setText(LocalDate.now().format(DateTimeFormatter.ofPattern("MMMM yyyy"))); // Set the current month
        gridPane.add(monthField, 1, rowIndex); // Add the text field to the grid pane at (1, rowIndex)

        rowIndex++;

        // Amount Label
        Label amountLabel = new Label("Amount of Charge:"); // Create a label for amount
        gridPane.add(amountLabel, 0, rowIndex); // Add the label to the grid pane at (0, rowIndex)

        // Amount Field
        amountField = new TextField(); // Create a text field for amount
        amountField.setPromptText("Enter amount of charge"); // Set a prompt text for the field
        gridPane.add(amountField, 1, rowIndex); // Add the text field to the grid pane at (1, rowIndex)

        rowIndex++;

        // Submit Button
        Button submitButton = new Button("Submit"); // Create a submit button
        gridPane.add(submitButton, 0, rowIndex, 2, 1); // Add the button to the grid pane spanning 2 columns

        // Add event handler for the submit button
        submitButton.setOnAction(event -> submitBillingInfo()); // Set the action to execute when the button is clicked
    }

    private void submitBillingInfo() {
        int customerId = customer.getCustomerId(); // Get the customer ID
        String month = monthField.getText(); // Get the entered month
        String amount = amountField.getText(); // Get the entered amount

        if (!month.isEmpty() && !amount.isEmpty()) { // Check if both month and amount fields are not empty
            try {
                File csvFile = new File("billing.csv"); // Create a file object for the CSV file

                // If the file doesn't exist, create it and add headers
                if (!csvFile.exists()) {
                    csvFile.createNewFile(); // Create a new file if it doesn't exist
                    BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile, true)); // Create a writer for the file
                    writer.write("CustomerId,Month,Bill\n"); // Write the header line to the file
                    writer.close(); // Close the writer
                }

                if (!functions.isNumber(amount)) { // Check if the entered amount is a valid number
                    functions.showMessage("Please enter a valid number for the amount.", false, false);
                    return; // Stop further execution if the amount is not valid
                }

                double final_amount = Double.parseDouble(amount);
                DecimalFormat decimalFormat = new DecimalFormat("#.##");
                final_amount = Double.parseDouble(decimalFormat.format(final_amount));

                // Read file and check if the month already exists
                List<String> lines = Files.readAllLines(csvFile.toPath());
                for (String line : lines) {
                    String[] fields = line.split(",");
                    if (fields.length >= 3 && fields[0].equals(String.valueOf(customerId)) && fields[1].equals(month)) {
                        // Month already exists
                        Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
                        alert.setTitle("Billing Form");
                        alert.setHeaderText("Already billed in this month." );
                        alert.setContentText("Do you want to add this price to existing price?");

                        Optional<ButtonType> result = alert.showAndWait();
                        if (result.get() == ButtonType.OK) {
                            // Add to the existing amount
                            double existingAmount = Double.parseDouble(fields[2]);
                            final_amount += existingAmount;
                            fields[2] = String.valueOf(final_amount);
                            lines.set(lines.indexOf(line), String.join(",", fields));
                            Files.write(csvFile.toPath(), lines);
                            return;
                        } else {
                            // Close the window
                            Stage stage = (Stage) monthField.getScene().getWindow();
                            stage.close();
                            return;
                        }
                    }
                }

                BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile, true)); // Create a writer for the file
                writer.write(customerId + "," + month + "," + final_amount + "\n"); // Write the billing information tothe file
                writer.close(); // Close the writer

                Alert alert = new Alert(Alert.AlertType.INFORMATION); // Create an information alert
                alert.setTitle("Billing Form");
                alert.setHeaderText(null);
                alert.setContentText("Bill submitted successfully.");
                alert.showAndWait();

                monthField.setText(LocalDate.now().format(DateTimeFormatter.ofPattern("MMMM yyyy"))); // Set the current month on the input field

                amountField.clear(); // Clear the amount field

            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING); // Create a warning alert
            alert.setTitle("Billing Form");
            alert.setHeaderText(null);
            alert.setContentText("Please fill in all the fields.");
            alert.showAndWait();
        }
    }

}
