## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/n8vm/SBVR/edit/master/README.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/n8vm/SBVR/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.


# SBVR
Slice Based Volume Renderer

Report
	* How I solved the assignment:
		* Most of this project was solved by following the GPU Gems article on slice based volume rendering. I implemented the camera controls myself, and recycled an old OpenGL/OpenCL project to speed up development.
	* What resources I used:
		* C++, Visual Studio 2017, CMake, OpenGL, OpenCL, GLFW, GLM, GLEW. 
	* Known bugs/TODO
		* Can't currently change out datasets without editing in code text. Need to read from command line args.
		* Can't alter the transfer function from within the application. The transfer function is a texture which must be edited externally.
		* OpenCL/OpenGL should be used to compute the proxy geometry.
		* I'd like to add a clipping plane, the ability to translate and rotate the volume, and the ability to translate the camera.
	* Program controls
		* Click and drag or arrow keys control camera.
		* +/- control camera dolly
		* yuiop control how many samples to take
		* qaws show camera direction, edge points, proxy geometry, and sampled volume respectively.
		* r resets the selected camera
		* 0 shows the tracked camera perspective
		* 1 shows the untracked camera perspective
