# Web Crawler Project

This project is a simple web crawler built using Python. It automatically scrapes PDF and ZIP files from a specified webpage and saves them to a local directory.

---

## **Features**
- Scrapes PDF and ZIP links from a webpage.
- Downloads the files and saves them to the specified directory.
- Supports logging download progress using `tqdm`.
- Configurable base URL and download directory.

---

## **Requirements**
The project depends on the following libraries, which are listed in the `requirements.txt` file:
- `requests`
- `beautifulsoup4`
- `lxml`
- `tqdm`
- `selenium` (optional, for dynamic web content)
- `PyPDF2` (optional, for PDF processing)

---

## **Dependencies**

1. **Get Anaconda**

2. **Create and activate**
    ```bash 
    conda create -n web_crawler python=3.9 -y
    conda activate web_crawler
    pip install -requirements.txt
    ```
3. **Configuration**
    ```python
    base_url = "https://example.com"  # Replace with the target URL
    save_dir = "./downloads"  # Directory to save files
    ```  
4. **Optional: Build is faster**
    ```bash
    chmod +x build.sh
    ./build.sh
    ```

