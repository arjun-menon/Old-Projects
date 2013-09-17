protectHTML
===========

This little tool lets you password-protect documents written in HTML just like you would password-protect a Word / LibreOffice / other document. **protectHTML** is written in Node.js and utilizes the [Stanford Javascript Crypto Library](http://bitwiseshiftleft.github.com/sjcl/) (SJCL) to encrypt and decrypt your HTML documents.

Usage
-----
First off, simply surround the part of your HTML you'd like to password-protect with: `<!-- ENCRYPTED BLOCK STARTS -->` and `<!-- ENCRYPTED BLOCK ENDS -->`. *protectHTML* will encrypt everything in between those two tags using the password you provide.

**protectHTML** takes three arguments in the following manner: `protectHTML input_file ouput_file password`. The first is the input HTML file, the second is the name of the file to which the generated HTML must be written, and the third is the password to be used for symmetrically encrypting and decrypting the marked out contents of the HTML.

License
-------
The [3-Clause BSD License](http://opensource.org/licenses/BSD-3-Clause)
