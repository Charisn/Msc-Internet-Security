/**

Program: Java FX Application

Filename: AnnualUsageForm.java

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
import javafx.scene.chart.*;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class AnnualUsageForm extends Application {

    private int customerId;  // The ID of the customer for which to display the annual energy usage

    public AnnualUsageForm(int customerId) {
        this.customerId = customerId;  // Initialize the customer ID
    }

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Annual Energy Usage");

        CategoryAxis xAxis = new CategoryAxis();  // Create a category axis for the X-axis
        NumberAxis yAxis = new NumberAxis();  // Create a number axis for the Y-axis
        BarChart<String, Number> barChart = new BarChart<>(xAxis, yAxis);  // Create a bar chart with the axes

        // [0] - START
        // https://docs.oracle.com/javase/8/javafx/api/javafx/scene/chart/XYChart.Series.html
        XYChart.Series<String, Number> series = new XYChart.Series<>();  // Create a series for data points
        series.setName("Monthly Energy Usage (kWh)");  // Set the name of the series
        // [0] - END

        //  [1] - START
        // https://www.guru99.com/buffered-reader-in-java.html
        try (BufferedReader reader = new BufferedReader(new FileReader("billing.csv"))) {
        //  [1] - END
            String line;
            boolean isFirstLine = true;  // Track if it's the first line (header line)
            while ((line = reader.readLine()) != null) {
                if (isFirstLine) {
                    isFirstLine = false;
                    continue;  // Skip the header line
                }
                String[] parts = line.split(",");
                int currentCustomerId = Integer.parseInt(parts[0]);  // Get the customer ID from the CSV
                if (currentCustomerId == customerId) {  // Check if the data belongs to the current customer
                    String month = parts[1];  // Get the month from the CSV
                    double energyUsage = Double.parseDouble(parts[2]);  // Get the energy usage from the CSV
                    // [0] - START
                    series.getData().add(new XYChart.Data<>(month, energyUsage));  // Add the data point to the series
                    // [0] - END
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        barChart.getData().add(series);  // Add the series to the bar chart

        VBox graphBox = new VBox(barChart);  // Create a vertical box to hold the bar chart
        graphBox.setPadding(new Insets(10));  // Set padding for the graph box
        Scene graphScene = new Scene(graphBox, 600, 400);  // Create a scene with the graph box
        primaryStage.setScene(graphScene);  // Set the scene to the primary stage
        primaryStage.show();  // Show the application window
    }
}
