<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@page import="com.db.*" %>
<%@page import="java.sql.*" %>
<%@page import="java.util.ArrayList" %>
<!DOCTYPE html>
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
	<title>Enterprise Resource Planning System</title>
	<!-- 부트스트랩 CSS 추가하기 -->
	<link rel="stylesheet" href="./css/bootstrap.min.css">
	<!-- 커스텀 CSS 추가하기 -->
	<link rel="stylesheet" href="./css/custom.css">
</head>
<%
	if(session.getAttribute("loginUser")!=null){
		 %>
		 <script>
			alert('로그인 상태입니다.');
			location.href="main.jsp";
		</script>
	<%	
	 }
%>
<body>
		<div class="col-md-12">
			<ul class="nav">
				<li class="nav-item">
					<a class="nav-link active" href="main.jsp"><h3>ERP System</h3></a>
				</li>
				<li class="nav-item">
					<a class="nav-link" href="profile.jsp">프로필</a>
				</li>
				<li class="nav-item dropdown ml-md-auto">
					 <a class="nav-link dropdown-toggle" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">회원정보</a>
					<div class="dropdown-menu dropdown-menu-right" aria-labelledby="navbarDropdownMenuLink">
						 <a class="dropdown-item" href="index.jsp">로그인</a>
						 <a class="dropdown-item" href="logoutAction.jsp">로그아웃</a>
						<div class="dropdown-divider">
						</div> <a class="dropdown-item" href="profile.jsp">프로필</a>
					</div>
				</li>
			</ul>
		</div>
	<section class="container mt-3" style="max-width: 560px;">
		<form method="post" action="loginAction.jsp">
			<div class="form-group">
				<label>사원ID</label>
				<input type="text" id="id" name="id" class="form-control">
			</div>
			<div class="form-group">
				<label>password</label>
				<input type="password" id="pwd" name="pwd" class="form-control">
			</div>
			<button type="submit" class="btn btn-primary">로그인</button>
		</form>
	</section>
	<!-- 제이쿼리 자바스크립트 추가하기 -->
	<script src="./js/jquery.min.js"></script>
	<!-- 파퍼 자바스크립트 추가하기 -->
	<script src="./js/popper.js"></script>
	<!-- 부트스트랩 자바스크립트 추가하기 -->
	<script src="./js/bootstrap.min.js"></script>
</body>
</html>