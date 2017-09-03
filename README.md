# Optimal LSB Substitution with ACO

<h2>Overview</h2>
<p>Optimal LSB Substitution with ACO is a program written in C++ and OpenCV used in research paper called <b>The Optimal LSB Substitution Matrix Technique for Image Hiding</b>, which was presented at 8th Knowledge Manangement International Conference 2016.</p>
<p>
Least Significant Bit (LSB) Substitution is a hiding information technique by embedded data (Secret Image) in the medium (Cover Image) and current information won’t be recognized. A safety and the quality of the embedded medium are the main point of hiding data because each bits of the medium will be replaced by bits of confidential data. The mediator of data can be changed and noticed and the quality of those data can be measured from Mean Square Error (MSE) and Peak Signal to Noise Ratio (PSNR). LSB substitution matrix was presented to increase the quality of the data which was decreased after the implantation. Every bits of the secret data will be counted as one pixel for changing the position within the matrix. In this current research, the researcher had presented the technique of finding optimal LSB substitution matrix by changing the position of each bits within the matrix integrated with changing the position with the pixel. When comparing the changing position of only with pixel, the changing position with pixel and bit give the better result for MSE and PSNR calculating.
</p>

<h2>Require</h2>
<p>- Building OpenCV on your machine</p>

<h2>Usage</h2>
<p>Compile and run the program with two arguments. The first one is a cover image (512 x 512 pixels) and the second is a secret image (256 x 256 pixels).</p>
<pre>./main coverImage.jpg secretImage.jpg</pre>
