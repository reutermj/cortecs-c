import re
from datetime import datetime
from git import Repo
repo = Repo()
head = repo.head.commit

if "Updating module version" not in head.message:
    today = datetime.now().strftime('%Y.%m.%d')

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
    index.commit("Updating module version to " + today)
