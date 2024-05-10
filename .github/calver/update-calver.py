import re
from datetime import datetime
from git import Repo
import subprocess

repo = Repo()
head = repo.head.commit

if "Updating module version" not in head.message:
    today = datetime.now().strftime('%Y.%m.%d')
    today_hyphens = datetime.now().strftime('%Y-%m-%d')

    with open("MODULE.bazel", "r") as f:
        lines = f.readlines()
    with open("MODULE.bazel", "w") as f:
        for line in lines:
            if 'module(name = "cortecs", version =' in line:
                f.write(re.sub(r"\d+\.\d+\.\d+", today, line))
            else:
                f.write(line)
    index = repo.index
    index.add(["MODULE.bazel"])
    message = "Updating module version to " + today
    index.commit(message)
    repo.remotes.origin.push(refspec="HEAD:version-bump/" + today_hyphens)
    pr_link = subprocess.check_output(['gh', 'pr', 'create', '--title', message, '--body', 'Automatic PR for calver bump. Please ignore and have a nice day', '--base', 'main', '--head', 'version-bump/' + today_hyphens]).decode("utf-8")
    for line in pr_link.splitlines():
        if "https://github.com/reutermj/cortecs-c/pull/" in line:
            pr_num = re.findall(r'\d+', line)[0]
            subprocess.run(['gh', 'pr', 'comment', pr_num, '--body', '/trunk merge'])

