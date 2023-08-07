import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.io.*;

import static org.junit.jupiter.api.Assertions.*;

public class EditBillFormTest {

    private EditBillForm form;
    private Billing billing;

    @BeforeEach
    public void setup() throws IOException {
        billing = new Billing();
        billing.setCustomerId(1);
        billing.setMonth("July");
        billing.setAmount(100.0);
        form = new EditBillForm(billing);

        // Setup a test CSV file
        BufferedWriter writer = new BufferedWriter(new FileWriter("testBilling.csv"));
        writer.write("customerId,month,amount\n");
        writer.write("1,July,100.0\n");
        writer.write("2,August,200.0\n");
        writer.close();
    }

    @Test
    public void testSaveChangesNewDate() {
        form.saveChanges("August", 100.0);
        form.saveChanges("May", 50.0);
        form.saveChanges("June", 10.0);
    }

    @Test
    public void testSaveChangesNewAmount() {
        form.saveChanges("July", 150.0);
        form.saveChanges("June", 250.0);
        form.saveChanges("May", 1205.0);
    }

    @Test
    public void testSaveChangesInvalidAmount() {
        form.saveChanges("July", -150.0);
        form.saveChanges("January", "Test Value");
        form.saveChanges("January", 0);
    }

    @Test
    public void testSaveChangesEmptyDate() {
        form.saveChanges("", 150.0);
    }
}
