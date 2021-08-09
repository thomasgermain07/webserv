<!DOCTYPE html>
<html lang="en">
<head>
	<title>WebServer</title>
	<link rel="stylesheet" href="./public/css/design.css">
</head>
<body>
	<?php include "partials/header.php"; ?>

	<div class="container">
		Please, be sure you have had the following 'hosts' to your <strong>/etc/hosts</strong> :

		<blockquote>
			127.0.0.1	dev.local<br>
			127.0.0.1	test.local<br>
			127.0.0.1	default.local<br>
			127.0.0.2	dev2.local<br>
			127.0.0.2	default2.local
		</blockquote>

		<h2>Get Form</h2>
		<?php
			if (isset($_GET['username']))
				echo "Welcome `" . $_GET['username'] . "` !";
		?>
		<form action="/index.php" method="get">
			<label for="fget_username">Username :</label>
			<input type="text" name="username" id="fget_username">
			<input type="submit" value="Send GET">
		</form>

		<h2>Post Form</h2>
		<?php
			if (isset($_POST['username']))
				echo "Welcome `" . $_POST['username'] . "` !";
		?>
		<form action="/index.php" method="post">
			<label for="fpost_username">Username :</label>
			<input type="text" name="username" id="fpost_username">
			<input type="submit" value="Send POST">
		</form>
	</div>

	<?php include "partials/footer.php"; ?>

	<script src="./public/js/timer.js"></script>
</body>
</html>
