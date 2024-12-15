package property;

import java.io.*;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class PropertyDatabaseManager {
    private static final String DB_URL = "jdbc:sqlite:propertydata.db";

    public static void createPropertyTable() {
        String createTableSQL = "CREATE TABLE IF NOT EXISTS properties (" +
                "type TEXT, " +
                "price REAL, " +
                "beds INTEGER, " +
                "baths REAL, " +
                "sqft INTEGER, " +
                "address TEXT, " +
                "latitude REAL, " +
                "longitude REAL" +
                ");";
        try (Connection conn = DriverManager.getConnection(DB_URL);
             Statement stmt = conn.createStatement()) {
            stmt.execute(createTableSQL); 
            System.out.println("Table `properties` created or already exists.");
        } catch (SQLException e) {
            System.out.println("Error creating table: " + e.getMessage());
            e.printStackTrace();
        }
    }


    public static void ingestCSVData(String csvFilePath) {
        String insertSQL = "INSERT INTO properties (type, price, beds, baths, sqft, address, latitude, longitude) " +
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

        try (BufferedReader br = new BufferedReader(new FileReader(csvFilePath));
             Connection conn = DriverManager.getConnection(DB_URL);
             PreparedStatement pstmt = conn.prepareStatement(insertSQL)) {

            String line = br.readLine(); // Skip header
            while ((line = br.readLine()) != null) {
                try {
                    String[] values = parseCSVLine(line);
                    if (values.length >= 8) {
                        pstmt.setString(1, values[0]);
                        pstmt.setDouble(2, Double.parseDouble(values[1]));
                        pstmt.setInt(3, Integer.parseInt(values[2]));
                        pstmt.setInt(4, (int) Math.floor(Double.parseDouble(values[3]))); 
                        pstmt.setInt(5, (int) Math.floor(Double.parseDouble(values[4]))); 
                        pstmt.setString(6, values[5]);
                        pstmt.setDouble(7, Double.parseDouble(values[6]));
                        pstmt.setDouble(8, Double.parseDouble(values[7]));
                        pstmt.executeUpdate();
                    } else {
                        System.out.println("Skipping invalid row: " + line);
                    }
                } catch (Exception e) {
                    System.out.println("Error parsing row: " + line + ". Error: " + e.getMessage());
                }
            }
            System.out.println("Data ingestion complete.");
        } catch (IOException | SQLException e) {
            System.out.println("Error ingesting data: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static String[] parseCSVLine(String line) {
        List<String> result = new ArrayList<>();
        boolean inQuotes = false;
        StringBuilder field = new StringBuilder();

        for (char c : line.toCharArray()) {
            if (c == '\"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                result.add(field.toString().trim());
                field.setLength(0);
            } else {
                field.append(c);
            }
        }
        result.add(field.toString().trim());
        return result.toArray(new String[0]);
    }
}
