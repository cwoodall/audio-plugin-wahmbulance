from invoke import task
import platform
import zipfile
import os
import shutil

project_name = "Wahmbulance"
with open("VERSION", "r") as f:
    project_version = f.read().strip()

if platform.system() == "Darwin":
    import dmgbuild

    os_name = "macOS"

elif platform.system() == "Linux":
    os_name = "Linux"
elif platform.system() == "Windows":
    os_name = "Windows"
else:
    raise Exception("Unknown OS")


@task
def __bootstrap_macos(c):
    c.run("brew install ccache ninja osxutils")


@task
def __bootstrap_linux(c):
    c.sudo("apt-get update")
    c.sudo(
        "apt install libasound2-dev libx11-dev libxinerama-dev libxext-dev libfreetype6-dev libwebkit2gtk-4.0-dev libglu1-mesa-dev xvfb fluxbox ninja-build libc++-dev libc++abi-dev libcurl4-openssl-dev"
    )


@task
def __bootstrap_windows(c):
    c.run("choco install ninja")


@task
def pluginval(c, build_dir="build"):
    pluginval_bin_name = {
        "macOS": "pluginval.app/Contents/MacOS/pluginval",
        "Linux": "./pluginval",
        "Windows": "pluginval.exe",
    }

    os.makedirs(build_dir, exist_ok=True)
    with c.cd(build_dir):
        c.run(
            f'curl -LO "https://github.com/Tracktion/pluginval/releases/download/v1.0.3/pluginval_{os_name}.zip"'
        )

    pwd = os.getcwd()
    os.chdir(build_dir)
    with zipfile.ZipFile(
        f"pluginval_{ os_name }.zip",
        "r",
    ) as zip_ref:
        zip_ref.extractall(pwd="build")
    os.chmod(f"{pluginval_bin_name[os_name]}", 0o777)
    c.run(
        f'{pluginval_bin_name[os_name]} --strictness-level 10 --verbose --validate "Wahmbulance_artefacts/Release/VST3"'
    )

    os.chdir(pwd)


@task
def bootstrap(c):
    bootstrap_map = {
        "macOS": __bootstrap_macos,
        "Linux": __bootstrap_linux,
        "Windows": __bootstrap_windows,
    }
    bootstrap_map[os_name](c)


@task
def build(
    c,
    cxx_flags="-std=c++20 -stdlib=libc++",
    j=12,
    build_type="Release",
    build_dir="build",
):
    # c refers to the terminal "context"
    print("building...")
    os.environ["CXXFLAGS"] = cxx_flags
    c.run(
        f"cmake -B {build_dir} -G Ninja "
        + f"-DCMAKE_BUILD_TYPE={build_type} "
        + "-DCMAKE_C_COMPILER_LAUNCHER=ccache "
        + "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache "
        + '-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" .'
    )
    c.run(f"cmake --build {build_dir} --config {build_type} --parallel {j}")
    print("Success!")


@task
def test(
    c,
    j=12,
    build_dir="build",
):
    with c.cd(build_dir):
        c.run(f"ctest --output-on-failure -j{j}")


@task
def package(
    c,
    build_dir="build",
    build_type="Release",
):
    product_name = f"{project_name}-{project_version}-{os_name}"
    artifacts_dir = f"{build_dir}/{project_name}_artefacts/{build_type}"
    vst3_path = f"{artifacts_dir}/VST3/{project_name}.vst3"

    def package_macos(_):
        # TODO #2 - Figure out how to add the icon to the vst3 again.
        dmgbuild.build_dmg(
            filename=f"{build_dir}/{project_name}-{project_version}-{os_name}.dmg",
            volume_name=project_name,
            settings_file="packaging/dmg.py",
            defines={
                "app": vst3_path,
                "icon": "packaging/wahmbulance.icns",
            },
        )

    def package_linux(_):
        shutil.make_archive(f"{build_dir}/{product_name}", "zip", f"{vst3_path}")

    def package_windows(c):
        c.run('iscc "packaging\installer.iss"')
        c.run(f'mv "packaging/Output/{product_name}.exe" "{build_dir}"')

    package_map = {
        "macOS": package_macos,
        "Linux": package_linux,
        "Windows": package_windows,
    }

    package_map[os_name](c)
