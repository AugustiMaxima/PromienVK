setlocal enabledelayedexpansion
@echo off
for %%v in (*.vert) do (
	echo %%v
	C:\VulkanSDK\1.2.135.0\Bin\glslc.exe %%v -o %%v.spv
)
for %%f in (*.frag) do (
	echo %%f
	C:\VulkanSDK\1.2.135.0\Bin\glslc.exe %%f -o %%f.spv
)