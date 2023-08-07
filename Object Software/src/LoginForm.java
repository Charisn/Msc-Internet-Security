/**

Program: Java FX Application

Filename: LoginForm.java

@author: © Charalampos Nikolaidis 

Course: BSc Computing

Module: Visual Object Software

Tutor: Theodoros Manavis

@version: 3.1

Date: 14/07/23

*/

import javafx.application.Application;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import javafx.geometry.Insets;
import java.util.Map;

import java.util.HashMap;

public class LoginForm extends Application {
    private TextField usernameField; // Text field for entering username
    private PasswordField passwordField; // Text field for entering password
    public Functions functions; // init functions

    public LoginForm() {
        functions = new Functions();
    }

    public static void main(String[] args) {
        launch(args);
    }
    
    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Login");

        VBox box = new VBox(20); // Vertical box for holding UI elements
        box.setAlignment(Pos.CENTER);
        box.setStyle("-fx-background-color: #ececec;");  // Light gray background

        GridPane gridPane = new GridPane(); // Grid pane for arranging UI elements in a grid
        gridPane.setAlignment(Pos.CENTER);
        gridPane.setHgap(10);
        gridPane.setVgap(10);
        gridPane.setPadding(new Insets(25, 25, 25, 25));

        Text sceneTitle = new Text("Welcome"); // Text element for displaying a welcome message
        sceneTitle.setFont(Font.font("Open-Sans", FontWeight.NORMAL, 20)); // Set the font and size
        sceneTitle.setFill(Color.DARKBLUE); // Set the text color
        box.getChildren().add(sceneTitle); // Add the welcome message to the vertical box

        Label usernameLabel = new Label("Username:"); // Label for the username field
        GridPane.setConstraints(usernameLabel, 0, 0); // Set the grid position of the label
        gridPane.getChildren().add(usernameLabel); // Add the label to the grid pane

        usernameField = new TextField(); // Text field for entering the username
        GridPane.setConstraints(usernameField, 1, 0); // Set the grid position of the text field
        gridPane.getChildren().add(usernameField); // Add the text field to the grid pane

        Label passwordLabel = new Label("Password:"); // Label for the password field
        GridPane.setConstraints(passwordLabel, 0, 1); // Set the grid position of the label
        gridPane.getChildren().add(passwordLabel); // Add the label to the grid pane

        passwordField = new PasswordField(); // Text field for entering the password
        GridPane.setConstraints(passwordField, 1, 1); // Set the grid position of the text field
        gridPane.getChildren().add(passwordField); // Add the text field to the grid pane

        Button loginButton = new Button("Login"); // Button for initiating login
        loginButton.setStyle("-fx-background-color: #008CBA; -fx-text-fill: white; -fx-font-size: 13px;"); // Set the button style
        loginButton.setOnAction(e -> authenticateUser(primaryStage)); // Set the action for the button
        GridPane.setConstraints(loginButton, 1, 2); // Set the grid position of the button
        gridPane.getChildren().add(loginButton); // Add the button to the grid pane

        box.getChildren().add(gridPane); // Add the grid pane to the vertical box

        Scene scene = new Scene(box, 350, 230); // Create a scene with the vertical box
        primaryStage.setScene(scene); // Set the scene for the primary stage
        primaryStage.show(); // Show the primary stage
    }

    public void authenticateUser(Stage primaryStage) {
        String username = usernameField.getText(); // Get the entered username
        String password = passwordField.getText(); // Get the entered password

        // Define valid usernames and passwords
        Map<String, String> validCredentials = new HashMap<>();
        validCredentials.put("admin", "admin");

        // Check if entered credentials match the valid ones
        if (validCredentials.containsKey(username) && validCredentials.get(username).equals(password)) {
            functions.showMessage("Login Successfull!", false, true);

            // Redirect to the CustomerList class
            CustomerList customerList = new CustomerList();
            customerList.start(primaryStage);
        } else {
            functions.showMessage("Invalid username or password", false, false);// Show an error message
        }
    }
}
