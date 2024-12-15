package property;

import javax.swing.*;
import java.awt.*;
import java.sql.*;
import java.util.ArrayList;

public class PropertyDisplay extends JFrame {
    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private String[] columnNames = {"Type", "Price", "Beds", "Baths", "SqFt", "Address", "Latitude", "Longitude"};
    private JTable table;
    private JPanel displayPanel;

  
    private JComboBox<String> typeComboBox;
    private JTextField leastBedsField;
    private JTextField leastBathsField;
    private JTextField leastSqFtField;
    private JComboBox<String> boroughComboBox;

    public PropertyDisplay() {

        super("Property Search and Filter");

        this.setSize(1024, 800);
        this.setLayout(new BorderLayout());
        this.add(createControlPanel(), BorderLayout.NORTH); 
        this.add(createDisplayPanel(), BorderLayout.CENTER);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }


    public JPanel createControlPanel() {
        JPanel controlPanel = new JPanel();
        controlPanel.setLayout(new FlowLayout());


        JLabel typeLabel = new JLabel("Type:");
        typeComboBox = new JComboBox<>(new String[]{
                "All", 
                "Condo for sale",
                "House for sale",
                "Townhouse for sale",
                "Multi-family Home for sale",
                "Co-op for sale",
                "Land for sale",
                "Foreclosure",
                "Pending",
                "Contingent"
        });

        JLabel leastBedsLabel = new JLabel("Least Beds:");
        leastBedsField = new JTextField(5);

        JLabel leastBathsLabel = new JLabel("Least Baths:");
        leastBathsField = new JTextField(5);

        JLabel leastSqFtLabel = new JLabel("Least SqFt:");
        leastSqFtField = new JTextField(7);

        JLabel boroughLabel = new JLabel("Borough:");
        boroughComboBox = new JComboBox<>(new String[]{"All", "Bronx", "Manhattan", "Brooklyn", "Queens", "Staten-Island"});

        JButton searchButton = new JButton("Search");
        searchButton.addActionListener(e -> refreshTable());

        JButton predictButton = new JButton("Predict");
        predictButton.addActionListener(e -> showPredictWindow());

   
        controlPanel.add(typeLabel);
        controlPanel.add(typeComboBox);
        controlPanel.add(leastBedsLabel);
        controlPanel.add(leastBedsField);
        controlPanel.add(leastBathsLabel);
        controlPanel.add(leastBathsField);
        controlPanel.add(leastSqFtLabel);
        controlPanel.add(leastSqFtField);
        controlPanel.add(boroughLabel);
        controlPanel.add(boroughComboBox);
        controlPanel.add(searchButton);
        controlPanel.add(predictButton);

        return controlPanel;
    }

 
    public JPanel createDisplayPanel() {
        displayPanel = new JPanel();
        refreshTable(); 
        return displayPanel;
    }


    private void refreshTable() {
        if (displayPanel != null) {
            displayPanel.removeAll();
        }

        String[][] data = fetchDatabaseDataWithFilter();
        table = new JTable(data, columnNames);
        table.setPreferredScrollableViewportSize(new Dimension(900, 400));
        JScrollPane scrollPane = new JScrollPane(table);
        table.setFillsViewportHeight(true);

        displayPanel.add(scrollPane);
        displayPanel.revalidate();
        displayPanel.repaint();
    }


    private String[][] fetchDatabaseDataWithFilter() {
        ArrayList<String[]> dataList = new ArrayList<>();
        String dbUrl = "jdbc:sqlite:propertydata.db";

   
        String selectedType = (String) typeComboBox.getSelectedItem();
        String leastBeds = leastBedsField.getText().trim();
        String leastBaths = leastBathsField.getText().trim();
        String leastSqFt = leastSqFtField.getText().trim();
        String borough = (String) boroughComboBox.getSelectedItem();

   
        StringBuilder query = new StringBuilder("SELECT * FROM properties WHERE 1=1");
        ArrayList<Object> params = new ArrayList<>();

        if (!"All".equals(selectedType)) {
            query.append(" AND type = ?");
            params.add(selectedType);
        }
        if (!leastBeds.isEmpty()) {
            query.append(" AND beds >= ?");
            params.add(Integer.parseInt(leastBeds)); 
        }
        if (!leastBaths.isEmpty()) {
            query.append(" AND baths >= ?");
            params.add(Integer.parseInt(leastBaths));
        }
        if (!leastSqFt.isEmpty()) {
            query.append(" AND sqft >= ?");
            params.add(Integer.parseInt(leastSqFt)); 
        }
        if (!"All".equals(borough)) {
            query.append(" AND address LIKE ?");
            params.add("%" + borough + "%");
        }

        try (Connection conn = DriverManager.getConnection(dbUrl);
             PreparedStatement pstmt = conn.prepareStatement(query.toString())) {

            // 设置查询参数
            for (int i = 0; i < params.size(); i++) {
                pstmt.setObject(i + 1, params.get(i));
            }

            ResultSet rs = pstmt.executeQuery();
            while (rs.next()) {
                String[] row = new String[columnNames.length];
                row[0] = rs.getString("type");
                row[1] = String.format("$%,.2f", rs.getDouble("price"));
                row[2] = String.valueOf(rs.getInt("beds"));
                row[3] = String.valueOf(rs.getInt("baths"));
                row[4] = String.valueOf(rs.getInt("sqft"));
                row[5] = rs.getString("address");
                row[6] = String.valueOf(rs.getDouble("latitude"));
                row[7] = String.valueOf(rs.getDouble("longitude"));
                dataList.add(row);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } catch (NumberFormatException e) {
            System.out.println("Invalid input for numeric fields.");
        }

        return dataList.toArray(new String[0][]);
    }


    private void showPredictWindow() {
        JFrame predictFrame = new JFrame("Predict Property Price");
        predictFrame.setSize(400, 300);
        predictFrame.setLayout(new GridLayout(7, 2));

        JLabel typeLabel = new JLabel("Type:");
        JComboBox<String> typeComboBox = new JComboBox<>(new String[]{
                "Condo for sale", "House for sale", "Townhouse for sale", "Multi-family Home for sale",
                "Co-op for sale", "Land for sale", "Foreclosure", "Pending", "Contingent"
        });

        JLabel boroughLabel = new JLabel("Borough:");
        JComboBox<String> boroughComboBox = new JComboBox<>(new String[]{
                "Bronx", "Manhattan", "Brooklyn", "Queens", "Staten-Island"
        });

        JLabel bedsLabel = new JLabel("Beds:");
        JTextField bedsField = new JTextField(5);

        JLabel bathsLabel = new JLabel("Baths:");
        JTextField bathsField = new JTextField(5);

        JLabel sqftLabel = new JLabel("SqFt:");
        JTextField sqftField = new JTextField(7);

        JButton predictButton = new JButton("Predict");
        JLabel resultLabel = new JLabel("Predicted Price: ");

        predictButton.addActionListener(e -> {
            try {
                String type = (String) typeComboBox.getSelectedItem();
                String borough = (String) boroughComboBox.getSelectedItem();
                int beds = Integer.parseInt(bedsField.getText().trim());
                double baths = Double.parseDouble(bathsField.getText().trim());
                int sqft = Integer.parseInt(sqftField.getText().trim());

                PricePredictor predictor = new PricePredictor();
                predictor.loadData("propertydata.db");
                double predictedPrice = predictor.predict(borough, type, beds, baths, sqft, 3);

                resultLabel.setText(String.format("Predicted Price: $%,.2f", predictedPrice));
            } catch (Exception ex) {
                resultLabel.setText("Invalid input. Please check your values.");
            }
        });

        predictFrame.add(typeLabel);
        predictFrame.add(typeComboBox);
        predictFrame.add(boroughLabel);
        predictFrame.add(boroughComboBox);
        predictFrame.add(bedsLabel);
        predictFrame.add(bedsField);
        predictFrame.add(bathsLabel);
        predictFrame.add(bathsField);
        predictFrame.add(sqftLabel);
        predictFrame.add(sqftField);
        predictFrame.add(predictButton);
        predictFrame.add(resultLabel);

        predictFrame.setVisible(true);
    }

    public static void main(String[] args) {
        PropertyDisplay display = new PropertyDisplay();
        display.setVisible(true);
    }
}
