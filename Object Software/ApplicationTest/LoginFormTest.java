import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import javafx.stage.Stage;

import static org.junit.jupiter.api.Assertions.*;

public class LoginFormTest {

    private LoginForm loginForm;
    
    @BeforeEach
    public void setup() {
        loginForm = new LoginForm();
    }
    
    @Test
    public void testAuthenticateUserWithCorrectCredentials() {
        loginForm.usernameField = new TextField("admin");
        loginForm.passwordField = new PasswordField();
        loginForm.passwordField.setText("admin");
        assertTrue(loginForm.authenticateUser(new Stage()));
    }

    @Test
    public void testAuthenticateUserWithIncorrectPassword() {
        loginForm.usernameField = new TextField("admin");
        loginForm.passwordField = new PasswordField();
        loginForm.passwordField.setText("wrongPassword");
        assertFalse(loginForm.authenticateUser(new Stage()));
    }

    @Test
    public void testAuthenticateUserWithNonexistentUsername() {
        loginForm.usernameField = new TextField("nonexistentUser");
        loginForm.passwordField = new PasswordField();
        loginForm.passwordField.setText("somePassword");
        assertFalse(loginForm.authenticateUser(new Stage()));
    }
    
    @Test
    public void testAuthenticateUserWithEmptyCredentials() {
        loginForm.usernameField = new TextField("");
        loginForm.passwordField = new PasswordField();
        loginForm.passwordField.setText("");
        assertFalse(loginForm.authenticateUser(new Stage()));
    }

    @Test
    public void testAuthenticateUserWithNullCredentials() {
        loginForm.usernameField = new TextField(null);
        loginForm.passwordField = new PasswordField();
        loginForm.passwordField.setText(null);
        assertFalse(loginForm.authenticateUser(new Stage()));
    }
}
