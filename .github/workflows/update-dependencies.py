import re
import os
from datetime import datetime
import git
import subprocess

# Get most recent commit
repo = git.Repo()
main = repo.heads.main

with open("MODULE.bazel", "r") as f:
    lines = f.readlines()

# 1) create a new branch
# 2) update MODULE.bazel for the single dependency update
# 3) push the branch
# 4) create a PR
# 5) kick off CI
# 6) switch back to main branch for next dependency
def replace_commit(line_number, old, new, name):
    # create new branch for the singel dependency update
    new_branch = repo.create_head(name, main.commit)
    new_branch.checkout()

    # write out new MODULE.bazel with the new commit
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
    
    # create the new commit for the dependency update
    message = "Updating {} dependency".format(name)
    print("creating commit:", message)
    index = repo.index
    index.add(["MODULE.bazel"])
    index.commit(message)

    # push to new branch
    today = datetime.now().strftime('%Y-%m-%d')
    remote_branch_name = "dependency-update/{}-{}".format(name, today)
    print("pushing to new branch:", remote_branch_name)
    branch_refspec = "HEAD:{}".format(remote_branch_name)
    repo.remotes.origin.push(refspec=branch_refspec)

    # create the PR
    print("creating pull request")
    pr_link = subprocess.check_output(['gh', 'pr', 'create', '--title', message, '--body', 'Automatic PR for dependency update.', '--base', 'main', '--head', remote_branch_name]).decode("utf-8")
    print(pr_link)
    
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

    if line.startswith("git_override(") or line.startswith("git_repository("):
        print("found dependency declaration on line", line_number)
        name = ""
        remote = ""
        branch = ""
        while line_number < len(lines):
            line = lines[line_number]
            line_number = line_number + 1
            if ")" in line or "build_file_content =" in line:
                print(line_number)
                missing = "commit"
                if name == "":
                    missing = missing + ", name"
                if branch == "":
                    missing = missing + ", branch"
                if remote == "":
                    missing = missing + ", remote"
                
                print("Found end of declaration but missing:", missing)

            if "name = " in line:
                current_name = line.split(sep='"')[1]
                if name == "":
                    name = current_name
                    print("Found name", name)
                else:
                    print("Found name", current_name, "but already found name", name)
                    break
                
            
            if "remote = " in line:
                current_remote = line.split(sep='"')[1]
                if remote == "":
                    remote = current_remote
                    print("Found remote", remote)
                else:
                    print("Found remote", current_remote, "but already found remote", remote)
                    break
            
            if "branch = " in line:
                current_branch = line.split(sep='"')[1]
                if branch == "":
                    branch = current_branch
                    print("Found branch", branch)
                else:
                    print("Found branch", current_branch, "but already found branch", branch)
                    break

            if "commit = " in line:
                if name == "":
                    print("name attribute not found. ignoring declaration")
                    break
                if remote == "":
                    print("remote attribute not found. ignoring declaration")
                    break
                if branch == "":
                    print("branch attribute not found. ignoring declaration")
                    break

                commit = line.split(sep='"')[1]
                print("Found commit", commit)
                print("Getting commit from tip of branch", branch, "at", remote)
                g = git.cmd.Git()
                new_commit = g.ls_remote(remote, branch).split()[0]
                
                print("Commit at tip of branch", new_commit)
                if new_commit != commit:
                    print("Updating", name, "dependency")
                    replace_commit(line_number - 1, commit, new_commit, name)
                else:
                    print("Dependency is up to date. ignoring")
                break
