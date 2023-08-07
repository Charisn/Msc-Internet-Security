import javafx.scene.input.KeyCode;
import javafx.stage.Stage;
import org.junit.Test;
import org.testfx.framework.junit.ApplicationTest;
import org.testfx.matcher.base.NodeMatchers;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.List;

import static org.junit.Assert.*;
import static org.testfx.api.FxAssert.verifyThat;

public class AddCustomerFormTest extends ApplicationTest {

    private AddCustomerForm addCustomerForm;

    @Override
    public void start(Stage stage) {
        addCustomerForm = new AddCustomerForm();
        addCustomerForm.start(stage);
    }

    @Test
    public void testFormWithValidInput() throws IOException {
        clickOn("#nameField").write("John Doe");
        clickOn("#phoneNumberField").write("1234567890");
        clickOn("#addressField").write("123 Test St, TestCity, TestCountry");
        clickOn("#energyBillingField").write("Test Billing");
        clickOn("#meterTypeField").write("Test Meter");
        clickOn("#addCustomerButton").click();

        List<String> lines = Files.readAllLines(new File("customers.csv").toPath());
        assertTrue(lines.contains("1,John Doe,1234567890,123 Test St, TestCity, TestCountry,Test Billing,Test Meter"));
    }

    @Test
    public void testFormWithEmptyFields() {
        clickOn("#addCustomerButton").click();

        // Check the error dialog title
        verifyThat(".dialog-pane .header", NodeMatchers.hasText("Error"));
    }

    @Test
    public void testFormWithExistingCustomer() throws IOException {
        clickOn("#nameField").write("John Doe");
        clickOn("#phoneNumberField").write("1234567890");
        clickOn("#addressField").write("123 Test St, TestCity, TestCountry");
        clickOn("#energyBillingField").write("Test Billing");
        clickOn("#meterTypeField").write("Test Meter");
        clickOn("#addCustomerButton").click();

        clickOn("#nameField").write("John Doe");
        clickOn("#phoneNumberField").write("1234567890");
        clickOn("#addressField").write("123 Test St, TestCity, TestCountry");
        clickOn("#energyBillingField").write("Test Billing");
        clickOn("#meterTypeField").write("Test Meter");
        clickOn("#addCustomerButton").click();

        List<String> lines = Files.readAllLines(new File("customers.csv").toPath());
        assertTrue(lines.contains("2,John Doe,1234567890,123 Test St, TestCity, TestCountry,Test Billing,Test Meter"));
    }
}
