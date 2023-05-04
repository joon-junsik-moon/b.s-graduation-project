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
	
	<!-- 테이블 검색 스크립트 -->
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
	<!-- 
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/js/bootstrap.min.js"></script> 
	-->
</head>
<!-- 항목 삭제 컨트롤러 -->
<body>
	<% 	
	SemiERPDAO s = new SemiERPDAO();
	s.dbConn();
	request.setCharacterEncoding("utf-8");
	boolean delresult;
	String str=request.getParameter("page");
	if(str.equals("sale")){
		delresult= s.DeleteSaleData(request.getParameter("delid"));
	}
	else if(str.equals("client")){
		delresult= s.DeleteAccountListtData(request.getParameter("delid"));
	}
	else if(str.equals("employee")){
		delresult= s.DeleteEmployeeData(request.getParameter("delid"));
	}
	else if(str.equals("materials")){
		delresult= s.DeleteMaterialsData(request.getParameter("delid"));
	}
	else if(str.equals("produce")){
		delresult= s.DeleteProductionManagementData(request.getParameter("delid"));
	}
	else if(str.equals("produce_list")){
		delresult= s.DeleteProductionListData(request.getParameter("delid"));
	}
	else if(str.equals("product")){
		delresult= s.DeleteProductData(request.getParameter("delid"));
	}
	else if(str.equals("purchase")){
		delresult= s.DeletePurchaseData(request.getParameter("delid"));
	}
	 %>
	 <script>
	 	location.href= "<%=request.getParameter("page") %>" + ".jsp";
	 </script>
</body>
</html>