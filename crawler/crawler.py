import os
import requests
from bs4 import BeautifulSoup
from tqdm import tqdm


def download_file(url, save_dir):
    """下载文件并保存到指定目录"""
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
    """爬取网页中的 PDF 和 ZIP 链接并下载"""
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
            # 如果是相对链接，补全成完整 URL
            if not file_url.startswith("http"):
                file_url = os.path.join(base_url, file_url)
            print(f"Found file: {file_url}")
            download_file(file_url, save_dir)


if __name__ == "__main__":
    # 示例网站
    base_url = "some web"  # 替换为目标网站
    save_dir = "./downloads"

    print("Starting the web crawler...")
    crawl_and_download(base_url, save_dir)
    print("Web crawler finished.")
