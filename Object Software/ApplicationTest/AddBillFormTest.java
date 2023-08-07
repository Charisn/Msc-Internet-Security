import javafx.scene.input.KeyCode;
import javafx.stage.Stage;
import org.junit.Test;
import org.testfx.framework.junit.ApplicationTest;
import org.testfx.matcher.base.NodeMatchers;
import org.testfx.matcher.control.TextInputControlMatchers;

import AddBillForm;
import Customer;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

import static org.junit.Assert.*;
import static org.testfx.api.FxAssert.verifyThat;

public class AddBillFormTest extends ApplicationTest {

    private AddBillForm addBillForm;
    private Customer customer;

    @Override
    public void start(Stage stage) {
        customer = new Customer(1, "John Doe", "1234567890", "123 Test St, TestCity, TestCountry", "Test Billing", "Test Meter");
        addBillForm = new AddBillForm(customer);
        addBillForm.start(stage);
    }

    @Test
    public void testFormWithValidInput() throws IOException {
        clickOn("#monthField").write("June 2023");
        clickOn("#amountField").write("120.5");
        clickOn("#submitButton").click();

        List<String> lines = Files.readAllLines(new File("billing.csv").toPath());
        assertTrue(lines.contains("1,June 2023,120.5"));
    }

    @Test
    public void testFormWithInvalidAmount() {
        clickOn("#monthField").write("June 2023");
        clickOn("#amountField").write("invalidAmount");
        clickOn("#submitButton").click();

        // Check the error dialog title
        verifyThat(".dialog-pane .header", NodeMatchers.hasText("Error"));
    }

    @Test
    public void testFormWithEmptyFields() {
        clickOn("#submitButton").click();

        // Check the warning dialog title
        verifyThat(".dialog-pane .header", NodeMatchers.hasText("Warning"));
    }

    @Test
    public void testFormWithExistingMonth() throws IOException {
        clickOn("#monthField").write("June 2023");
        clickOn("#amountField").write("120.5");
        clickOn("#submitButton").click();
        clickOn("#monthField").write("June 2023");
        clickOn("#amountField").write("150.5");
        clickOn("#submitButton").click();

        press(KeyCode.ENTER); // Press enter to confirm the addition to existing price

        List<String> lines = Files.readAllLines(new File("billing.csv").toPath());
        assertTrue(lines.contains("1,June 2023,271"));
    }

    @Test
    public void testFormWithExistingMonthCancel() throws IOException {
        clickOn("#monthField").write("June 2023");
        clickOn("#amountField").write("120.5");
        clickOn("#submitButton").click();
        clickOn("#monthField").write("June 2023");
        clickOn("#amountField").write("150.5");
        clickOn("#submitButton").click();

        press(KeyCode.ESCAPE); // Press escape to cancel the addition

        List<String> lines = Files.readAllLines(new File("billing.csv").toPath());
        assertTrue(lines.contains("1,June 2023,120.5"));
        assertFalse(lines.contains("1,June 2023,271"));
    }
}
