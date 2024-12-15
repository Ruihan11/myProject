package property;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class PricePredictor {
    private List<Object[]> data; 


    public void loadData(String dbFilePath) {
        data = new ArrayList<>();
        String url = "jdbc:sqlite:" + dbFilePath;

        try (Connection conn = DriverManager.getConnection(url)) {
            if (conn == null) {
                System.out.println("Failed to connect to the database.");
                return;
            }

            String query = "SELECT address, type, beds, baths, sqft, price FROM properties";
            try (Statement stmt = conn.createStatement();
                 ResultSet rs = stmt.executeQuery(query)) {

                while (rs.next()) {
                    String address = rs.getString("address");
                    String type = rs.getString("type");
                    double beds = rs.getDouble("beds");
                    double baths = rs.getDouble("baths");
                    double sqft = rs.getDouble("sqft");
                    double price = rs.getDouble("price");

            
                    data.add(new Object[]{address, type, beds, baths, sqft, price});
                }
            }
            System.out.println("Data loaded successfully from database.");
        } catch (SQLException e) {
            System.out.println("Error loading data: " + e.getMessage());
            e.printStackTrace();
        }
    }


    private boolean matchesBorough(String address, String borough) {
        if (borough.equalsIgnoreCase("All")) {
            return true;
        }
        return address.toLowerCase().contains(borough.toLowerCase());
    }


    private double distance(Object[] record, String borough, String type, double[] inputFeatures) {
        double sum = 0;

      
        if (!matchesBorough((String) record[0], borough)) {
            sum += 5000; 
        }

      
        if (!type.equals(record[1])) {
            sum += 10000; 
        }

   
        for (int i = 2; i <= 4; i++) {
            sum += Math.pow((double) record[i] - inputFeatures[i - 2], 2);
        }

        return Math.sqrt(sum);
    }


    public double predict(String borough, String type, int beds, double baths, int sqft, int k) {
        if (data == null || data.isEmpty()) {
            System.out.println("No data available. Please load data first.");
            return -1;
        }

        double[] inputFeatures = {beds, baths, sqft};
        List<Object[]> distances = new ArrayList<>();

  
        for (Object[] record : data) {
            double dist = distance(record, borough, type, inputFeatures);
            distances.add(new Object[]{dist, record[5]}); 
        }

    
        distances.sort((a, b) -> Double.compare((double) a[0], (double) b[0]));

      
        double priceSum = 0;
        for (int i = 0; i < k; i++) {
            priceSum += (double) distances.get(i)[1];
        }

    
        return priceSum / k;
    }

    public static void main(String[] args) {
        PricePredictor predictor = new PricePredictor();

    
        String dbFilePath = "propertydata.db";

        predictor.loadData(dbFilePath);

      
        int k = 5; 
        String borough = "Manhattan"; 
        String type = "Townhouse for sale";
        double predictedPrice = predictor.predict(borough, type, 8, 8, 5000, k); // 包括 Borough 信息
        System.out.printf("Predicted price for %s in %s (8 beds, 8 baths, 5000 sqft, k=%d): $%,.2f%n",
                type, borough, k, predictedPrice);
    }
}
