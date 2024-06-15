/**

Program: Java FX Application

Filename: AddCustomerForm.java

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
import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class AddCustomerForm extends Application {

    private TextField nameField;
    private TextField phoneNumberField;
    private TextField addressField;
    private TextField energyBillingField;
    private TextField meterTypeField;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Add Customer Form");

        GridPane gridPane = createFormPane();
        addUIControls(gridPane);

        Scene scene = new Scene(gridPane, 400, 250);
        primaryStage.setScene(scene);

        primaryStage.show();
    }

    private GridPane createFormPane() {
        GridPane gridPane = new GridPane();
        gridPane.setPadding(new Insets(20));
        gridPane.setHgap(10);
        gridPane.setVgap(10);

        return gridPane;  // Return the created GridPane
    }

    private void addUIControls(GridPane gridPane) {
        // Name Label
        Label nameLabel = new Label("Name:");
        gridPane.add(nameLabel, 0, 0);

        // Name Field
        nameField = new TextField();
        nameField.setPromptText("Enter customer name");
        gridPane.add(nameField, 1, 0);

        // Phone Number Label
        Label phoneNumberLabel = new Label("Phone Number:");
        gridPane.add(phoneNumberLabel, 0, 1);

        // Phone Number Field
        phoneNumberField = new TextField();
        phoneNumberField.setPromptText("Enter phone number");
        gridPane.add(phoneNumberField, 1, 1);

        // Address Label
        Label addressLabel = new Label("Address:");
        gridPane.add(addressLabel, 0, 2);

        // Address Field
        addressField = new TextField();
        addressField.setPromptText("Enter address");
        gridPane.add(addressField, 1, 2);

        // Energy Billing Label
        Label energyBillingLabel = new Label("Energy Billing:");
        gridPane.add(energyBillingLabel, 0, 3);

        // Energy Billing Field
        energyBillingField = new TextField();
        energyBillingField.setPromptText("Enter energy billing");
        gridPane.add(energyBillingField, 1, 3);

        // Meter Type Label
        Label meterTypeLabel = new Label("Meter Type:");
        gridPane.add(meterTypeLabel, 0, 4);

        // Meter Type Field
        meterTypeField = new TextField();
        meterTypeField.setPromptText("Enter meter type");
        gridPane.add(meterTypeField, 1, 4);

        // Add Customer Button
        Button addCustomerButton = new Button("Add Customer");
        addCustomerButton.setOnAction(event -> addCustomer());
        gridPane.add(addCustomerButton, 0, 5, 2, 1);
    }

    private void addCustomer() {
        String name = nameField.getText();
        String phoneNumber = phoneNumberField.getText();
        String address = addressField.getText();
        String energyBilling = energyBillingField.getText();
        String meterType = meterTypeField.getText();

        if (name.isEmpty() || phoneNumber.isEmpty() || address.isEmpty() || energyBilling.isEmpty() || meterType.isEmpty()) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Add Customer Form");
            alert.setHeaderText(null);
            alert.setContentText("Please fill in all the fields.");
            alert.showAndWait();
            return; // Stop the saving process if any field is empty
        }
        
        try {
            File csvFile = new File("customers.csv");

            if (!csvFile.exists()) {
                csvFile.createNewFile();
                BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile, true));
                writer.write("CustomerID,Name,PhoneNumber,Address,EnergyBilling,MeterType\n");
                writer.close();
            }

            int customerID = getLastCustomerID() + 1; // Generate new customer ID

            // this is the same as saveCustomersToCSV but i could not make it work for this exact use without making many changes to the method.
            BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile, true));
            writer.write(customerID + "," + name + "," + phoneNumber + "," + address + "," + energyBilling + "," + meterType);
            writer.newLine();
            writer.close();

            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("Add Customer Form");
            alert.setHeaderText(null);
            alert.setContentText("Customer added successfully.");
            alert.showAndWait();

            // Clear the form fields
            nameField.clear();
            phoneNumberField.clear();
            addressField.clear();
            energyBillingField.clear();
            meterTypeField.clear();

        } catch (IOException e) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Add Customer Form");
            alert.setHeaderText(null);
            alert.setContentText("An error occurred while adding the customer. Please try again.");
            alert.showAndWait();
            e.printStackTrace();
        }
    }

    private int getLastCustomerID() throws IOException {
        File csvFile = new File("customers.csv");

        if (!csvFile.exists()) {
            return 0; // If the file doesn't exist, assume no customers yet
        }

        BufferedReader reader = new BufferedReader(new FileReader(csvFile));
        String line;
        List<Integer> customerIDs = new ArrayList<>();

        // Skip header line
        reader.readLine();

        while ((line = reader.readLine()) != null) {
            String[] data = line.split(",");
            if (data.length >= 1) { // Check if the data length is correct
                int customerID = Integer.parseInt(data[0]);
                customerIDs.add(customerID);
            }
        }
        reader.close();

        if (customerIDs.isEmpty()) {
            return 0; // If no customer IDs found, assume no customers yet
        }

        // Find the maximum customer ID
        int maxCustomerID = customerIDs.stream().mapToInt(Integer::intValue).max().getAsInt();
        return maxCustomerID;
    }
}
