package property;

public class Property {
    private String type;
    private double price;
    private int beds;
    private double baths;
    private int sqft;
    private String address;
    private double latitude;
    private double longitude;

    public Property(String type, double price, int beds, double baths, int sqft, String address, double latitude, double longitude) {
        this.type = type;
        this.price = price;
        this.beds = beds;
        this.baths = baths;
        this.sqft = sqft;
        this.address = address;
        this.latitude = latitude;
        this.longitude = longitude;
    }

    public String getType() {
        return type;
    }

    public double getPrice() {
        return price;
    }

    public int getBeds() {
        return beds;
    }

    public double getBaths() {
        return baths;
    }

    public int getSqft() {
        return sqft;
    }

    public String getAddress() {
        return address;
    }

    public double getLatitude() {
        return latitude;
    }

    public double getLongitude() {
        return longitude;
    }

    @Override
    public String toString() {
        return String.format("%s: $%,.2f, %d beds, %.1f baths", type, price, beds, baths);
    }
}
