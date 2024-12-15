package property;

import java.io.File;

public class Main {
    public static void main(String[] args) {
   
        String databasePath = "propertydata.db";
        File dbFile = new File(databasePath);
        if (dbFile.exists()) {
            if (dbFile.delete()) {
                System.out.println("Old database file deleted successfully: " + dbFile.getAbsolutePath());
            } else {
                System.out.println("Failed to delete the old database file: " + dbFile.getAbsolutePath());
                return; 
            }
        } else {
            System.out.println("No existing database file found to delete.");
        }

        
        System.out.println("Creating database table...");
        PropertyDatabaseManager.createPropertyTable();

        
        String relativePath = "dataset.csv"; 
        System.out.println("CSV Absolute Path: " + new File(relativePath).getAbsolutePath());
        if (!new File(relativePath).exists()) {
            System.out.println("CSV file not found at: " + relativePath);
            return;
        }
        System.out.println("Importing CSV data... this may take a while...");
        PropertyDatabaseManager.ingestCSVData(relativePath);

      
        System.out.println("Launching GUI...");
        javax.swing.SwingUtilities.invokeLater(() -> {
            PropertyDisplay display = new PropertyDisplay();
            display.setVisible(true);
        });
    }
}
