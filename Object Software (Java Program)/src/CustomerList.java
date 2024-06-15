/**

Program: Java FX Application

Filename: CustomerList.java

@author: © Charalampos Nikolaidis 

Course: BSc Computing

Module: Visual Object Software

Tutor: Theodoros Manavis

@version: 3.1

Date: 14/07/23

*/

import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

public class CustomerList extends Application {

    private ObservableList<Customer> customerList; // A list to store customer objects
    private TableView<Customer> customerTable; // A table to display customer data
    private Functions functions; // An instance of a helper class

    public static void main(String[] args) {
        launch(args);
    }

    public CustomerList() {
        functions = new Functions(); // Create an instance of the helper class
    }

    @Override
    public void start(Stage primaryStage) {
        customerList = FXCollections.observableArrayList(functions.readCustomersFromCSV()); // Read customer data from CSV file
        customerTable = new TableView<>(); // Create an empty table

        primaryStage.setTitle("Customer Management");

        GridPane gridPane = functions.createForm(30, 10); // Create a grid pane for UI layout
        addUIControls(gridPane); // Add UI controls to the grid pane

        Scene scene = new Scene(gridPane, 500, 400); // Create a scene with the grid pane
        primaryStage.setScene(scene);

        primaryStage.show(); // Show the application window
    }

    private void addUIControls(GridPane gridPane) {
        int rowIndex = 0;

        // Search Label
        Label searchLabel = new Label("Search:"); // Create a label for the search field
        gridPane.add(searchLabel, 1, rowIndex);

        // Create a search field
        TextField searchField = new TextField();
        searchField.setPromptText("Enter account number or name");
        searchField.setPrefWidth(200);

        searchField.setOnKeyReleased(e -> {
            String searchText = searchField.getText(); // Get the text entered in the search field
            if (searchText != null && !searchText.isEmpty()) {
                List<Customer> searchResults = searchCustomers(searchText); // Search for customers matching the entered text
                updateCustomerTableView(searchResults); // Update the table view with the search results
            } else {
                updateCustomerTableView(customerList); // If search text is empty, show all customers in the table
            }
        });
        gridPane.add(searchField, 2, rowIndex);

        // Refresh Button
        Button refreshButton = new Button("Refresh"); // Create a refresh button
        refreshButton.setOnAction(e -> {
            customerList = FXCollections.observableArrayList(functions.readCustomersFromCSV()); // Refresh the customer list from CSV
            updateCustomerTableView(customerList); // Update the table view with the refreshed list
        });
        gridPane.add(refreshButton, 0, rowIndex);

        rowIndex++;

        // Table View
        customerTable.setPrefWidth(450); // Set width of the table view
        customerTable.setPrefHeight(450); // Set height of the table view
        TableColumn<Customer, Integer> idColumn = new TableColumn<>("Customer ID"); // Create a column for customer ID
        idColumn.setCellValueFactory(new PropertyValueFactory<>("customerId")); // Bind the ID column to the"customerId" property of Customer
        TableColumn<Customer, String> nameColumn = new TableColumn<>("Name"); // Create a column for customer name
        nameColumn.setCellValueFactory(new PropertyValueFactory<>("name")); // Bind the name column to the "name" property of Customer class
        customerTable.getColumns().add(idColumn); // Add the ID column to the table
        customerTable.getColumns().add(nameColumn); // Add the name column to the table

        // Details Button Column
        TableColumn<Customer, Void> detailsColumn = new TableColumn<>(""); // Create a column for details button
        detailsColumn.setCellFactory(param -> new TableCell<>() {
            private final Button btnDetails = new Button("Details"); // Create a details button
            {
                btnDetails.setOnAction((ActionEvent event) -> {
                    Customer customer = getTableView().getItems().get(getIndex()); // Get the selected customer from the table
                    showCustomerDetails(customer); // Show the details of the selected customer
                });
            }

            @Override
            public void updateItem(Void item, boolean empty) {
                super.updateItem(item, empty);
                if (empty) {
                    setGraphic(null);
                } else {
                    setGraphic(btnDetails);
                }
            }
        });
        customerTable.getColumns().add(detailsColumn); // Add the details column to the table

        customerTable.widthProperty().addListener((observable, oldValue, newValue) -> {
            double tableWidth = newValue.doubleValue();
            double buttonWidth = 200;
            double remainingWidth = tableWidth - buttonWidth;

            detailsColumn.setPrefWidth(remainingWidth);
        });
        detailsColumn.setStyle("-fx-alignment: center-right;");

        gridPane.add(customerTable, 0, rowIndex, 4, 1); // Add the table to the grid pane
        // Define two rows for buttons
        HBox addCusBtn = new HBox();
        HBox delCusBtn = new HBox();
        HBox addBillBtn = new HBox();
        HBox showBillBtn = new HBox();

        // Add Customer Button
        Button addCustomerButton = new Button("Add Customer"); // Create an "Add Customer" button
        addCustomerButton.setOnAction(event -> {
            AddCustomerForm addCustomerForm = new AddCustomerForm();
            addCustomerForm.start(new Stage());
        });
        addCusBtn.getChildren().add(addCustomerButton); // Add the "Add Customer" button to the button row

        // Delete Customer Button
        Button deleteCustomerButton = new Button("Delete Customer"); // Create a "Delete Customer" button
        deleteCustomerButton.setStyle("-fx-background-color: #FF0000; -fx-text-fill: #FFFFFF;");
        deleteCustomerButton.setOnAction(event -> {
            Customer selectedCustomer = customerTable.getSelectionModel().getSelectedItem(); // Get the selected customer from the table
            if (selectedCustomer != null) {
                Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
                alert.setTitle("Delete Customer");
                alert.setHeaderText("Confirm Delete");
                alert.setContentText("Are you sure you want to delete this customer?");

                Optional<ButtonType> result = alert.showAndWait();
                if (result.isPresent() && result.get() == ButtonType.OK) {
                    customerList.remove(selectedCustomer); // Remove the selected customer from the list
                    removeBillsByCustomerID(selectedCustomer.getCustomerId()); // Remove bills associated with the customer
                    updateCustomerTableView(customerList); // Update the table view with the modified list
                    saveCustomersToCSV(); // Save the updated customer list to CSV
                }
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setTitle("No Customer Selected");
                alert.setHeaderText(null);
                alert.setContentText("Please select a customer to delete.");
                alert.showAndWait();
            }
        });

        delCusBtn.getChildren().add(deleteCustomerButton); // Add the "Delete Customer" button to the button row
        // Add Bill Button
        Button addBillButton = new Button("Add Bill"); // Create an "Add Bill" button
        addBillButton.setOnAction(event -> {
            Customer selectedCustomer = customerTable.getSelectionModel().getSelectedItem(); // Get the selected customer from the table
            if (selectedCustomer != null) {
                AddBillForm addBillForm = new AddBillForm(selectedCustomer);
                addBillForm.start(new Stage());
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setTitle("No Customer Selected");
                alert.setHeaderText(null);
                alert.setContentText("Please select a customer to add a bill.");
                alert.showAndWait();
            }
        });
        addBillBtn.getChildren().add(addBillButton); // Add the "Add Bill" button to the button row

        // Show Bills Button
        Button showBillsButton = new Button("Show Bills"); // Create a "Show Bills" button
        showBillsButton.setOnAction(event -> {
            Customer selectedCustomer = customerTable.getSelectionModel().getSelectedItem(); // Get the selected customer from the table
            if (selectedCustomer != null) {
                BillListForm billListForm = new BillListForm(selectedCustomer);
                billListForm.start(new Stage());
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setTitle("No Customer Selected");
                alert.setHeaderText(null);
                alert.setContentText("Please select a customer to show bills.");
                alert.showAndWait();
            }
        });
        showBillBtn.getChildren().add(showBillsButton); // Add the "Show Bills" button to the button row

        // Add button rows to the grid pane
        gridPane.add(addCusBtn, 0, 3);
        gridPane.add(delCusBtn, 0, 4);

        gridPane.add(addBillBtn, 1, 3);
        gridPane.add(showBillBtn, 1, 4);

        GridPane.setColumnSpan(addBillBtn, 3);
        GridPane.setColumnSpan(showBillBtn, 3);
        addBillBtn.setAlignment(Pos.CENTER_RIGHT);
        showBillBtn.setAlignment(Pos.CENTER_RIGHT);

        if (customerList.isEmpty()) {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("No Customers");
            alert.setHeaderText(null);
            alert.setContentText("No customers to show.");
            alert.showAndWait();
        } else {
            updateCustomerTableView(customerList); // Update the table view with the customer list
        }
    }

    private void removeBillsByCustomerID(int customerID) {
        File billingFile = new File("billing.csv"); // Get the billing CSV file
        List<String> updatedLines = new ArrayList<>();

        try {
            if (billingFile.exists()) {
                FileReader reader = new FileReader(billingFile);
                BufferedReader bufferedReader = new BufferedReader(reader);

                String line;
                boolean isFirstLine = true;
                while ((line = bufferedReader.readLine()) != null) {
                    if (isFirstLine) {
                        isFirstLine = false;
                        continue; // Skip the first line (header)
                    }

                    String[] billData = line.split(",");
                    if (billData.length >= 1) {
                        int billCustomerID = Integer.parseInt(billData[0]);

                        // Check if the bill's customerID matches the selected customer's customerID
                        if (billCustomerID != customerID) {
                            updatedLines.add(line); // Add the bill line to the updated list
                        }
                    }
                }

                bufferedReader.close();

                // Write the updated lines back to the billing.csv file
                FileWriter writer = new FileWriter(billingFile, false); // Overwrite existing file
                for (String updatedLine : updatedLines) {
                    writer.write(updatedLine + "\n");
                }
                writer.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        } catch (NumberFormatException e) {
            e.printStackTrace();
            System.out.println("Invalid customer ID format in billing.csv");
        }
    }

    private void updateCustomerTableView(List<Customer> customers) {
        customerTable.getItems().setAll(customers); // Set the table view's items to the provided list of customers
    }

    private List<Customer> searchCustomers(String searchText) {
        return customerList.stream()
                .filter(customer -> {
                    if (functions.isNumber(searchText)) {
                        return customer.getCustomerId() == Integer.parseInt(searchText); // Filter customers by ID
                    } else {
                        String searchLower = searchText.toLowerCase();
                        return customer.getName().toLowerCase().contains(searchLower); // Filter customers by name
                    }
                })
                .collect(Collectors.toList()); // Convert the filtered customers to a list
    }

    private void saveCustomersToCSV() {

        File csvFile = new File("customers.csv"); // Get the customers CSV file

        try {
            FileWriter writer = new FileWriter(csvFile, false); // Overwrite existing file
            writer.write("CustomerID,Name,PhoneNumber,Address,EnergyBilling,MeterType\n"); // CSV header

            for (Customer customer : customerList) {
                writer.write(
                        customer.getCustomerId() + "," +
                                customer.getName() + "," +
                                customer.getPhoneNumber() + "," +
                                customer.getAddress() + "," +
                                customer.getEnergyBilling() + "," +
                                customer.getMeterType() + "\n"); // Write customer data to the file
            }

            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void showCustomerDetails(Customer customer) {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("Customer Details");
        alert.setHeaderText(null);
        alert.setContentText(
                "Name: " + customer.getName() + "\n" +
                        "Phone Number: " + customer.getPhoneNumber() + "\n" +
                        "Address: " + customer.getAddress() + "\n" +
                        "Energy Billing: " + customer.getEnergyBilling() + "\n" +
                        "Meter Type: " + customer.getMeterType());
        alert.showAndWait();
    }
}
