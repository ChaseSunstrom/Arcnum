import os
import shutil
import sys
import subprocess
import requests
from packaging import version
from zipfile import ZipFile

def get_latest_release_tag(github_repo):
    api_url = f"https://api.github.com/repos/{github_repo}/tags"
    response = requests.get(api_url)
    if response.status_code == 200:
        tags = response.json()
        if tags:
            latest_tag = tags[0]['name']
            return latest_tag
        else:
            print("No tags found in the repository.")
            return None
    else:
        print(f"Failed to fetch tags. Status Code: {response.status_code}")
        return None

def get_version_from_file(file_path):
    if os.path.exists(file_path):
        with open(file_path, 'r') as file:
            content = file.readline().strip()
            version_number = content.split()[1]
            return version_number
    return "0.0.0"


def download_and_extract_repo(github_repo, temp_dir, tag_name):
    repo_url = f'https://github.com/{github_repo}/archive/refs/tags/{tag_name}.zip'
    response = requests.get(repo_url)
    if response.status_code != 200:
        print(f"Failed to download the repository. Status Code: {response.status_code}")
        sys.exit(1)

    zip_path = os.path.join(temp_dir, 'download.zip')
    with open(zip_path, 'wb') as file:
        file.write(response.content)

    with ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(temp_dir)
    os.remove(zip_path)

    extracted_dir_name = [name for name in os.listdir(temp_dir) if os.path.isdir(os.path.join(temp_dir, name))][0]
    extracted_path = os.path.join(temp_dir, extracted_dir_name)
    return extracted_path

def update_spark_if_newer(spark_dir, temp_dir, github_repo):
    tag_name = get_latest_release_tag(github_repo)
    if not tag_name:
        print("Unable to get the latest version tag.")
        return

    latest_version_tag = tag_name.lstrip('v')
    local_version = get_version_from_file(os.path.join(spark_dir, 'version.txt'))

    # Check if the local version is less than the latest version
    if local_version < latest_version_tag:
        print(f"Updating Spark from version {local_version} to {latest_version_tag}.")
        extracted_path = download_and_extract_repo(github_repo, temp_dir, tag_name)
        if os.path.exists(spark_dir):
            shutil.rmtree(spark_dir)
        shutil.move(extracted_path, spark_dir)
        print("Spark has been updated successfully.")

        # Write the new version to the version.txt file
        version_file_path = os.path.join(spark_dir, 'version.txt')
        with open(version_file_path, 'w') as version_file:
            version_file.write(f"VERSION {latest_version_tag}")
        print(f"Updated version.txt to VERSION {latest_version_tag}.")
    else:
        print(f"Local Spark is up to date with version {local_version}.")


if __name__ == "__main__":
    current_project_dir = os.getcwd()  # Directory of the current project
    current_project_spark_dir = os.path.join(current_project_dir, 'Spark')  # Spark directory in the current project
    temp_dir = os.path.join(current_project_dir, 'TempSparkDownload')  # Temporary directory for Spark download
    github_repo = 'ChaseSunstrom/Spark'  # GitHub repository for Spark

    os.makedirs(temp_dir, exist_ok=True)
    update_spark_if_newer(current_project_spark_dir, temp_dir, github_repo)

    if os.path.exists(temp_dir) and not os.listdir(temp_dir):
        os.rmdir(temp_dir)
