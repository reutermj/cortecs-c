import re
from datetime import datetime
from git import Repo
import subprocess

# Get most recent commit
repo = Repo()
head = repo.head.commit

# Only bump version if new commits were pushed in the last day
if "Updating module version" not in head.message:
    today = datetime.now()
    # Date formatted for the module version number
    today_module_formatted = today.strftime('%Y.%m.%d')
    # Date formatted for the branch name
    today_branch_formatted = today.strftime('%Y-%m-%d')

    with open("MODULE.bazel", "r") as f:
        lines = f.readlines()

    # Iterate each line of MODULE.bazel, search for the module definition line,
    # and replace the version number with today's date.
    with open("MODULE.bazel", "w") as f:
        for line in lines:
            if 'module(name = "cortecs", version =' in line:
                updated_line = re.sub(r"\d+\.\d+\.\d+", today_module_formatted, line)
                print("- ", line, end="")
                print("+ ", updated_line, end="")
                f.write(updated_line)
            else:
                print(line, end="")
                f.write(line)

    # Create a new commit

    index = repo.index
    index.add(["MODULE.bazel"])
    message = "Updating module version to " + today_module_formatted
    index.commit(message)
    print("created commit:", message)

    # Push the commit to a new branch
    branch_refspec = "HEAD:version-bump/" + today_branch_formatted
    repo.remotes.origin.push(refspec=branch_refspec)
    print("pushed to branch", branch_refspec)

    # Create a pull request from the new branch to main
    # The output of this gh command is a link to the PR
    # the link contains the PR number which is used to merge the PR
    print("createing PR")
    pr_link = subprocess.check_output(['gh', 'pr', 'create', '--title', message, '--body', 'Automatic PR for calver bump. Please ignore and have a nice day', '--base', 'main', '--head', 'version-bump/' + today_branch_formatted]).decode("utf-8")
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
                approve_output = subprocess.check_output(['gh', 'pr', 'review', pr_num, '--approve']).decode("utf-8")
                print(approve_output)
else:
    print("No new commits. Ignoring version bump")
