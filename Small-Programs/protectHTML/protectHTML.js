#!/usr/bin/env node

fs = require('fs');
sjcl = require('./sjcl.js');

in_file = process.argv[2];
out_file = process.argv[3];
passwd = process.argv[4];

if( !in_file || !out_file || !passwd ) {
	console.log("Usage: protectHTML input_file ouput_file password");
	process.exit(1);
}

text = fs.readFileSync(in_file, 'ascii');

block_starts = "<!-- ENCRYPTED BLOCK STARTS -->";
block_ends = "<!-- ENCRYPTED BLOCK ENDS -->";

function jsDecryptor(ciphertext) {
	script_start = '\n<script type="text/javascript">\n';
	script_end = '\n</script>\n';
	// use cookies instead of GET: http://stackoverflow.com/questions/11344531/pure-javascript-store-object-in-cookie
	doc_writer = "try { document.write( sjcl.decrypt(purl().param('key'), '" + ciphertext + "') ); } catch(err) " 
		+ "{ document.write('<form method=\"get\">Access Denied. Password: <input type=\"password\" name=\"key\"> &nbsp; <input type=\"submit\" value=\"Decrypt\"></form>'); }"
	return script_start + doc_writer + script_end;
}

s_i = text.search( new RegExp(block_starts, 'g') );
e_i = text.search( new RegExp(block_ends,   'g') );

if ( s_i > 0 && e_i > s_i ) {
	pre_plaintext = text.slice(0, s_i)
	plaintext = text.slice(s_i + block_starts.length, e_i)
	post_plaintext = text.slice(e_i + block_ends.length, text.length)

	ciphertext = sjcl.encrypt(passwd, plaintext);

	output_text = pre_plaintext + jsDecryptor(ciphertext) + post_plaintext

	out_file_handle = fs.openSync(out_file, 'w');
	fs.writeSync(out_file_handle, output_text)
	fs.closeSync(out_file_handle);
}
else {
	console.log("Please surround the block to be encrypted with " + 
		block_starts + " and " + block_ends + " in your markup file.")
}
