import requests
import json
import webbrowser as web
import os
url = "http://update-api.simple-scm.ringotek.cn"

if __name__ == '__main__':
    print('正在检查更新...')

    local_path = os.path.realpath(__file__)
    local_path = os.path.dirname(local_path)

    # 父目录的父目录
    grandpa_path = os.path.dirname(local_path)
    # print(local_path)

    response = requests.get(url)

    response = json.loads(response.text)

    version = response["latest_version"]
    version_number = int(response['latest_version_number'])

    with open(os.path.join(os.path.dirname(grandpa_path), 'static/LOCAL_VERSION'), 'r') as f:
        local_version = int(f.read())
    
    if version_number > local_version:
        # 存在可用更新
        print("存在可用更新，正在获取更新信息...")
        data = {
            "version": local_version
        }
        response = requests.post(url, data=json.dumps(data))
        # 返回的版本信息应该是一个列表
        
        response = json.loads(response.text.encode("utf8"))
        
        sorted(response, key= lambda x:x.__getitem__("version_number"))
        response.reverse()
        print("您将要升级到版本： "+response[0]['version_name']+"\n版本号： "+str(response[0]['version_number']))

        print("\n以下是更新日志：")

        for x in response:
            print("版本：{}\t版本号：{}".format(x['version_name'], x['version_number']))
            print("发布日期：{}".format(x['published_date']))
            print("更新内容：\n")
            print(x['detail'])
            print("\n")
        

        cmd = input("是否更新？(Y/N) >>").lower()
        if cmd != 'y':
            print("操作已取消。")
            exit(0)
        else:
            # 打开更新界面
            print("下载页面已打开，请进入页面下载！")
            web.open(response[0]['link'])

    else:
        print("您的Simple-SCM已经是最新版！")

        
        