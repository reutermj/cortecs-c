import re
import os
from datetime import datetime
import git
import subprocess

# Get most recent commit
repo = git.Repo()
main = repo.heads.main
#new_branch = repo.create_head('flecs', main.commit)
#new_branch.checkout()

#main.checkout()

with open("MODULE.bazel", "r") as f:
    lines = f.readlines()

def replace_commit(line_number, old, new, name):
    new_branch = repo.create_head(name, main.commit)
    new_branch.checkout()
    with open("MODULE.bazel", "w") as f:
        for i in range(len(lines)):
            line = lines[i]
            if i == line_number:
                updated_line = line.replace(old, new)
                print("-", line, end="")
                print("+", updated_line, end="")
                f.write(updated_line)
            else:
                print(line, end="")
                f.write(line)
    
    index = repo.index
    index.add(["MODULE.bazel"])
    message = "Updating {} dependency".format(name)
    index.commit(message)
    print(message)

    today = datetime.now().strftime('%Y-%m-%d')
    remote_branch_name = "dependency-update/{}-{}".format(name, today)
    branch_refspec = "HEAD:{}".format(remote_branch_name)
    repo.remotes.origin.push(refspec=branch_refspec)

    pr_link = subprocess.check_output(['gh', 'pr', 'create', '--title', message, '--body', 'Automatic PR for dependency update.', '--base', 'main', '--head', remote_branch_name]).decode("utf-8")
    for line in pr_link.splitlines():
        # Find the line with the PR number
        if "https://github.com/reutermj/cortecs-c/pull/" in line:
            numbers = re.findall(r'\d+', line)
            if len(numbers) != 1:
                print("Found too many numbers in PR line number")
            else:
                # Get the PR number
                pr_num = numbers[0]
                print("Found PR number:", pr_num)
                # Comment on the PR to initiate a merge
                print("Kicking off trunk merge")
                subprocess.run(['gh', 'pr', 'comment', pr_num, '--body', '/trunk merge'])
    
    main.checkout()

line_number = 0
while line_number < len(lines):
    line = lines[line_number]
    line_number = line_number + 1

    if "git_repository" in line or "git_override" in line:
        name = ""
        remote = ""
        branch = ""
        commit = ""
        while line_number < len(lines):
            line = lines[line_number]
            line_number = line_number + 1
            if "name = " in line:
                name = line.split(sep='"')[1]
                print(name)
            
            if "remote = " in line:
                remote = line.split(sep='"')[1]
                print(remote)
            
            if "branch = " in line:
                branch = line.split(sep='"')[1]
                print(branch)

            if "commit = " in line:
                commit = line.split(sep='"')[1]
                g = git.cmd.Git()
                new_commit = g.ls_remote(remote, branch).split()[0]
                print(commit)
                print(new_commit)
                if new_commit != commit:
                    replace_commit(line_number - 1, commit, new_commit, name)
                break
