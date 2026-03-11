#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>

using namespace std;

// ========== ENUM for Transaction Type ==========
enum TransactionType { WITHDRAWAL, DEPOSIT, BALANCE_INQUIRY, PIN_CHANGE };

// ========== USER CLASS ==========
class User {
private:
    string name;
    int userID;
    string mobileNumber;
    string email;

public:
    // Constructors
    User() : name(""), userID(0), mobileNumber(""), email("") {}
    
    User(string name, int id, string mobile, string email) {
        this->name = name;
        this->userID = id;
        this->mobileNumber = mobile;
        this->email = email;
    }
    
    // Getters
    string getName() const { return name; }
    int getUserID() const { return userID; }
    string getMobileNumber() const { return mobileNumber; }
    string getEmail() const { return email; }
    
    // Setters
    void setName(string n) { name = n; }
    void setMobileNumber(string mobile) { mobileNumber = mobile; }
    void setEmail(string e) { email = e; }
    
    // Display user info
    void displayUserInfo() const {
        cout << "\n=== USER DETAILS ===" << endl;
        cout << "Name: " << name << endl;
        cout << "User ID: " << userID << endl;
        cout << "Mobile: " << mobileNumber << endl;
        cout << "Email: " << email << endl;
    }
};

// ========== ACCOUNT CLASS ==========
class Account {
private:
    int accountNumber;
    User* user;
    double balance;
    int pin;

public:
    // Constructors
    Account() : accountNumber(0), user(nullptr), balance(0.0), pin(0) {}
    
    Account(int accNo, User* userPtr, double initialBalance, int pinCode) {
        this->accountNumber = accNo;
        this->user = userPtr;
        this->balance = initialBalance;
        this->pin = pinCode;
    }
    
    // Getters
    int getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    User* getUser() const { return user; }
    
    // PIN validation
    bool validatePin(int enteredPin) const {
        return (pin == enteredPin);
    }
    
    // Withdraw money
    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "Invalid amount! Please enter positive amount." << endl;
            return false;
        }
        
        if (amount > balance) {
            cout << "Insufficient balance! Current balance: $" << balance << endl;
            return false;
        }
        
        if (amount > 10000) {
            cout << "Daily withdrawal limit is $10000!" << endl;
            return false;
        }
        
        balance -= amount;
        cout << "Withdrawal successful! Amount: $" << amount << endl;
        cout << "Remaining balance: $" << balance << endl;
        return true;
    }
    
    // Deposit money
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposit successful! New balance: $" << balance << endl;
        } else {
            cout << "Invalid deposit amount!" << endl;
        }
    }
    
    // Display account info
    void displayAccountInfo() const {
        cout << "\n=== ACCOUNT DETAILS ===" << endl;
        cout << "Account Number: " << accountNumber << endl;
        cout << "Current Balance: $" << balance << endl;
        if (user != nullptr) {
            user->displayUserInfo();
        }
    }
    
    // Change PIN
    bool changePin(int oldPin, int newPin) {
        if (validatePin(oldPin)) {
            if (newPin >= 1000 && newPin <= 9999) {
                pin = newPin;
                cout << "PIN changed successfully!" << endl;
                return true;
            } else {
                cout << "PIN must be 4 digits!" << endl;
                return false;
            }
        } else {
            cout << "Incorrect old PIN!" << endl;
            return false;
        }
    }
};

// ========== TRANSACTION CLASS ==========
class Transaction {
private:
    struct TransactionRecord {
        int accountNumber;
        TransactionType type;
        double amount;
        string timestamp;
    };
    
    vector<TransactionRecord> transactionHistory;

public:
    // Get current timestamp
    string getCurrentTimestamp() const {
        time_t now = time(0);
        char* dt = ctime(&now);
        return string(dt);
    }
    
    // Process withdrawal
    bool processWithdrawal(Account& account, double amount) {
        if (account.withdraw(amount)) {
            TransactionRecord record;
            record.accountNumber = account.getAccountNumber();
            record.type = WITHDRAWAL;
            record.amount = amount;
            record.timestamp = getCurrentTimestamp();
            transactionHistory.push_back(record);
            return true;
        }
        return false;
    }
    
    // Process deposit
    bool processDeposit(Account& account, double amount) {
        account.deposit(amount);
        TransactionRecord record;
        record.accountNumber = account.getAccountNumber();
        record.type = DEPOSIT;
        record.amount = amount;
        record.timestamp = getCurrentTimestamp();
        transactionHistory.push_back(record);
        return true;
    }
    
    // Show balance
    void showBalance(const Account& account) {
        cout << "\n=== BALANCE INQUIRY ===" << endl;
        cout << "Current Balance: $" << account.getBalance() << endl;
        
        TransactionRecord record;
        record.accountNumber = account.getAccountNumber();
        record.type = BALANCE_INQUIRY;
        record.amount = 0;
        record.timestamp = getCurrentTimestamp();
        transactionHistory.push_back(record);
    }
    
    // Update mobile number
    bool updateMobileNumber(User& user) {
        string newMobile;
        cout << "Enter new mobile number (10 digits): ";
        cin >> newMobile;
        
        if (newMobile.length() == 10) {
            user.setMobileNumber(newMobile);
            cout << "Mobile number updated successfully!" << endl;
            
            TransactionRecord record;
            record.accountNumber = 0;
            record.type = PIN_CHANGE;
            record.amount = 0;
            record.timestamp = getCurrentTimestamp();
            transactionHistory.push_back(record);
            return true;
        } else {
            cout << "Invalid mobile number! Must be 10 digits." << endl;
            return false;
        }
    }
    
    // Show transaction history
    void showTransactionHistory(int accountNumber) const {
        cout << "\n=== TRANSACTION HISTORY ===" << endl;
        bool found = false;
        
        for (const auto& record : transactionHistory) {
            if (record.accountNumber == accountNumber) {
                found = true;
                cout << "Type: ";
                switch(record.type) {
                    case WITHDRAWAL: cout << "Withdrawal"; break;
                    case DEPOSIT: cout << "Deposit"; break;
                    case BALANCE_INQUIRY: cout << "Balance Inquiry"; break;
                    case PIN_CHANGE: cout << "PIN Change/Mobile Update"; break;
                }
                
                if (record.amount > 0) {
                    cout << " | Amount: $" << record.amount;
                }
                cout << " | " << record.timestamp;
            }
        }
        
        if (!found) {
            cout << "No transactions found for this account." << endl;
        }
    }
};

// ========== FUNCTION PROTOTYPES ==========
int showATMMenu();
bool authenticateUser(vector<Account>& accounts, int& accountIndex);
void waitForEnter();

// ========== MAIN FUNCTION ==========
int main() {
    // Create sample users and accounts
    User user1("John Doe", 1001, "9876543210", "john@email.com");
    User user2("Jane Smith", 1002, "9876543211", "jane@email.com");
    
    vector<Account> accounts;
    accounts.push_back(Account(5001, &user1, 5000.00, 1234));
    accounts.push_back(Account(5002, &user2, 10000.00, 5678));
    
    Transaction transaction;
    int loggedInAccountIndex = -1;
    
    cout << "\n=====================================" << endl;
    cout << "   WELCOME TO ATM SIMULATION SYSTEM  " << endl;
    cout << "=====================================" << endl;
    
    // Authentication
    if (!authenticateUser(accounts, loggedInAccountIndex)) {
        cout << "\nToo many failed attempts! Exiting..." << endl;
        return 0;
    }
    
    Account& currentAccount = accounts[loggedInAccountIndex];
    User* currentUser = currentAccount.getUser();
    
    // Main ATM loop
    int choice;
    do {
        choice = showATMMenu();
        
        switch(choice) {
            case 1: // Balance Inquiry
                transaction.showBalance(currentAccount);
                break;
            
            case 2: { // Cash Withdrawal
                double amount;
                cout << "\nEnter amount to withdraw: $";
                cin >> amount;
                
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input! Please enter a number." << endl;
                    break;
                }
                
                transaction.processWithdrawal(currentAccount, amount);
                break;
            }
            
            case 3: // User Details
                currentUser->displayUserInfo();
                break;
            
            case 4: // Update Mobile Number
                transaction.updateMobileNumber(*currentUser);
                break;
            
            case 5: // Mini Statement
                transaction.showTransactionHistory(currentAccount.getAccountNumber());
                break;
            
            case 6: { // Deposit
                double amount;
                cout << "\nEnter amount to deposit: $";
                cin >> amount;
                
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input! Please enter a number." << endl;
                    break;
                }
                
                transaction.processDeposit(currentAccount, amount);
                break;
            }
            
            case 7: { // Change PIN
                int oldPin, newPin;
                cout << "\nEnter old PIN: ";
                cin >> oldPin;
                cout << "Enter new PIN (4 digits): ";
                cin >> newPin;
                
                currentAccount.changePin(oldPin, newPin);
                break;
            }
            
            case 8: // Exit
                cout << "\nThank you for using ATM. Goodbye!" << endl;
                cout << "=====================================" << endl;
                break;
            
            default:
                cout << "\nInvalid choice! Please try again." << endl;
        }
        
        if (choice != 8) {
            waitForEnter();
        }
        
    } while (choice != 8);
    
    return 0;
}

// ========== AUTHENTICATION FUNCTION ==========
bool authenticateUser(vector<Account>& accounts, int& accountIndex) {
    int accNumber;
    int pin;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;
    
    while (attempts < MAX_ATTEMPTS) {
        cout << "\n--- LOGIN ---" << endl;
        cout << "Account Number: ";
        cin >> accNumber;
        cout << "PIN: ";
        cin >> pin;
        
        // Find account
        for (int i = 0; i < accounts.size(); i++) {
            if (accounts[i].getAccountNumber() == accNumber) {
                if (accounts[i].validatePin(pin)) {
                    accountIndex = i;
                    cout << "\nLogin successful! Welcome, " 
                         << accounts[i].getUser()->getName() << "!" << endl;
                    return true;
                } else {
                    cout << "Invalid PIN!" << endl;
                    attempts++;
                    break;
                }
            }
        }
        
        if (accountIndex == -1) {
            cout << "Account not found!" << endl;
            attempts++;
        }
        
        cout << "Attempts remaining: " << (MAX_ATTEMPTS - attempts) << endl;
    }
    
    return false;
}

// ========== ATM MENU ==========
int showATMMenu() {
    int choice;
    cout << "\n=====================================" << endl;
    cout << "           ATM MAIN MENU             " << endl;
    cout << "=====================================" << endl;
    cout << "1. Balance Inquiry" << endl;
    cout << "2. Cash Withdrawal" << endl;
    cout << "3. User Details" << endl;
    cout << "4. Update Mobile Number" << endl;
    cout << "5. Mini Statement" << endl;
    cout << "6. Deposit" << endl;
    cout << "7. Change PIN" << endl;
    cout << "8. Exit" << endl;
    cout << "=====================================" << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    return choice;
}

// ========== WAIT FOR ENTER ==========
void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}