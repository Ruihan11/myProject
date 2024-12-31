import os
import requests
from bs4 import BeautifulSoup
from tqdm import tqdm


def download_file(url, save_dir):
    """Download file and save it to the specified directory."""
    file_name = os.path.join(save_dir, url.split("/")[-1])
    response = requests.get(url, stream=True)
    
    if response.status_code == 200:
        total_size = int(response.headers.get('content-length', 0))
        with open(file_name, "wb") as f:
            for data in tqdm(response.iter_content(1024), total=total_size // 1024, unit="KB"):
                f.write(data)
        print(f"Downloaded: {file_name}")
    else:
        print(f"Failed to download: {url}")


def crawl_and_download(base_url, save_dir):
    """Scrape PDF and ZIP links from the webpage and download them."""
    os.makedirs(save_dir, exist_ok=True)
    response = requests.get(base_url)
    
    if response.status_code != 200:
        print(f"Failed to fetch the webpage: {base_url}")
        return

    soup = BeautifulSoup(response.text, "html.parser")
    links = soup.find_all("a", href=True)
    
    for link in links:
        file_url = link["href"]
        if file_url.endswith((".pdf", ".zip")):
            # If it's a relative link, construct a full URL
            if not file_url.startswith("http"):
                file_url = os.path.join(base_url, file_url)
            print(f"Found file: {file_url}")
            download_file(file_url, save_dir)


if __name__ == "__main__":
    # Example website
    base_url = "https://example.com"  # website
    save_dir = "./downloads"  # save directory

    print("Starting the web crawler...")
    crawl_and_download(base_url, save_dir)
    print("Web crawler finished.")
