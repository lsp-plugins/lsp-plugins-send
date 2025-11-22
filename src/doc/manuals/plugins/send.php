<?php
	plugin_header();
	$m      =   ($PAGE == 'send_mono') ? 'm' : 's';
?>

<p>
	This is a plugin that allows to send signal via the special primitive - shared memory segment (called 'shared memory link').
</p>
<p>
	It may be useful for inteacting between different DAWs, plugin formats or establish connection between
	audio software when good straightforward way is not possible.
</p>
<p>
	Note that transferring audio via the shared memory link may introduce some random latency, so it is important to not 
	to consider that transmitter and receiver will be always sample-precise synchronized.
</p>

<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the output signal is similar to input signal. That does not mean
		that the plugin is not working.
	</li>
	<li><b>Input</b> - the gain applied to the input signal.</li>
	<li><b>Send</b> - the gain applied to the signal passed to the shared memory link.</li>
	<li><b>Link</b> - the name of the shared memory link connection.</li>
	<li><b>Output</b> - the gain applied to the output signal.</li>
</ul>
