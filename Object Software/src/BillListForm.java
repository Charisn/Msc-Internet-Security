/**

Program: Java FX Application

Filename: BillListForm.java

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
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.stage.Stage;
import javafx.scene.control.cell.PropertyValueFactory;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class BillListForm extends Application {

    private TableView<Billing> billsTableView;  // A table to display bill data
    private TableColumn<Billing, String> monthColumn;  // A column for months
    private TableColumn<Billing, Double> billColumn;  // A column for bill amounts
    private Customer customer;  // The customer for whom the bills are displayed

    public BillListForm(Customer customer) {
        this.customer = customer;  // Initialize the customer
    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Bill List Form");

        BorderPane borderPane = new BorderPane();  // Create a border pane for layout
        VBox vBox = createFormPane();  // Create a vertical box for UI controls
        addUIControls(vBox);  // Add UI controls to the vertical box

        borderPane.setCenter(vBox);  // Set the vertical box as the center of the border pane

        Scene scene = new Scene(borderPane, 400, 400); // Increase the height to 400
        primaryStage.setScene(scene);

        primaryStage.show();  // Show the application window
    }

    private VBox createFormPane() {
        VBox vBox = new VBox();
        vBox.setPadding(new Insets(20));
        vBox.setSpacing(10);

        return vBox;  // Return the created vertical box
    }

    private void addUIControls(VBox vBox) {
        HBox headerBox = new HBox();
        headerBox.setAlignment(Pos.CENTER_RIGHT);

        Label billListLabel = new Label("Bills for: ");  // Create a label for bill list

        Label billListLabelName = new Label(customer.getName().toUpperCase());  // Create a label for the customer's name
        billListLabelName.setStyle("-fx-font-weight: bold;");  // Apply bold styling

        Button refreshButton = new Button("Refresh");  // Create a refresh button
        refreshButton.setOnAction(e -> refreshBills());  // Set the action to refresh the bill list

        Pane spacer = new Pane();
        HBox.setHgrow(spacer, Priority.ALWAYS);  // Use the spacer to push the refresh button to the right

        headerBox.getChildren().addAll(billListLabel, billListLabelName, spacer, refreshButton);  // Add UI controls to the header box
        vBox.getChildren().add(headerBox);  // Add the header box to the main vertical box

        billsTableView = new TableView<>();  // Create a table view for bills

        monthColumn = new TableColumn<>("MONTH");  // Create a column for months
        monthColumn.setCellValueFactory(new PropertyValueFactory<>("month"));  // Bind the column to the "month" property of Billing class

        billColumn = new TableColumn<>("BILL");  // Create a column for bill amounts
        billColumn.setCellValueFactory(new PropertyValueFactory<>("amount"));  // Bind the column to the "amount" property of Billing class

        billsTableView.getColumns().add(monthColumn);  // Add the columns to the table view
        billsTableView.getColumns().add(billColumn);

        vBox.getChildren().add(billsTableView);  // Add the table view to the main vertical box
        loadBills();  // Load the bill data

        Button editBillButton = new Button("Edit Bill");  // Create an "Edit Bill" button
        editBillButton.setOnAction(e -> openEditForm());  // Set the action to open the edit bill form
        vBox.getChildren().add(editBillButton);  // Add the button to the main vertical box

        Button annualUsageButton = new Button("Show Annual Energy Usage Graph");  // Create a button to show the annual energy usage graph
        annualUsageButton.setOnAction(e -> displayAnnualUsageGraph());  // Set the action to display the graph
        vBox.getChildren().add(annualUsageButton);  // Add the button to the main vertical box
    }

    private void loadBills() {
        try (BufferedReader reader = new BufferedReader(new FileReader("billing.csv"))) {
            String line;
            ObservableList<Billing> data = FXCollections.observableArrayList();
            boolean isFirstLine = true;  // Track if it's the first line (header line)
            while ((line = reader.readLine()) != null) {
                if (isFirstLine) {
                    isFirstLine = false;
                    continue;  // Skip the header line
                }
                String[] parts = line.split(",");
                if (Integer.parseInt(parts[0]) == customer.getCustomerId()) {  // Check if the bill belongs to the current customer
                    String month = parts[1];
                    double amount = Double.parseDouble(parts[2]);
                    data.add(new Billing(customer.getCustomerId(), month, amount));  // Add the bill to the data list
                }
            }

            // Set the CellValueFactory for billColumn and monthColumn
            billColumn.setCellValueFactory(new PropertyValueFactory<>("amount"));
            monthColumn.setCellValueFactory(new PropertyValueFactory<>("month"));

            billsTableView.setItems(data);  // Set the data list as the items of the table view
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    private void openEditForm() {
        Billing selectedBill = billsTableView.getSelectionModel().getSelectedItem();  // Get the selected bill from the table
        if (selectedBill != null) {
            EditBillForm editForm = new EditBillForm(selectedBill);  // Create an instance of the edit bill form
            Stage editStage = new Stage();
            editForm.start(editStage);  // Start the edit bill form in a new stage
            editStage.setOnCloseRequest(event -> {
                refreshBills();  // Refresh the bill list when the edit form is closed
                editStage.close();
            });
        }
    }

    private void refreshBills() {
        billsTableView.getItems().clear();  // Clear the table view items
        loadBills();  // Load the bills again to refresh the table view
    }
    
    private void displayAnnualUsageGraph() {
        AnnualUsageForm annualUsageForm = new AnnualUsageForm(customer.getCustomerId());  // Create an instance of the annual usage form
        Stage annualUsageStage = new Stage();
        annualUsageForm.start(annualUsageStage);  // Start the annual usage form in a new stage
    }
}
