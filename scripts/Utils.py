import requests
import sys
import time
from fake_useragent import UserAgent

def downloadFile(url, filePath):
    with open(filePath, 'wb') as f:
        ua = UserAgent()
        headers = { 'User-Agent': ua.chrome }
        response = requests.get(url, headers=headers, stream=True)
        total = response.headers.get('content-length')
        if total is None:
            f.write(response.content)
        else:
            downloaded = 0
            total = int(total)
            startTime = time.time()
            for data in response.iter_content(chunk_size=max(int(total / 1000), 1024 * 1024)):
                downloaded += len(data)
                f.write(data)
                progress = downloaded / total
                done = int(50 * progress)
                percentage = progress * 100
                elapsedTime = time.time() - startTime
                avgKBPS = downloaded / 1024 / (elapsedTime + 1e-7)
                avgSpeedString = '{:.2f} KB/s'.format(avgKBPS)
                if avgKBPS > 1024:
                    avgMBPS = avgKBPS / 1024
                    avgSpeedString = '{:.2f} MB/s'.format(avgMBPS)

                sys.stdout.write('\r[{}{}] {:.2f}% ({})    '.format('█' * done, ' ' * (50 - done), percentage, avgSpeedString))
                sys.stdout.flush()

    sys.stdout.write('\n')

def yesOrNo():
    while True:
        reply = str(input('[Y/N]: ')).lower().strip()
        if reply[:1] == 'y':
            return True
        
        if reply[:1] == 'n':
            return False
