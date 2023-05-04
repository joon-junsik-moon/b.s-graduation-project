<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@page import="com.db.*" %>
<%@page import="java.sql.*" %>
<%@page import="java.util.ArrayList" %>
<%@page import ="java.text.DecimalFormat" %>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
</head>
<body>
<%
	SemiERPDAO s = new SemiERPDAO();
	s.dbConn();
	request.setCharacterEncoding("utf-8");
	ArrayList<SaleDTO> list=s.SaleList();
	if((!request.getParameter("saleId").equals("") || request.getParameterValues("autoIncrease")!=null)
			&& !request.getParameter("amount").equals("") 
			&& !request.getParameter("saleDate").equals("") 
			&& !request.getParameter("unitprice").equals("") ){
		String sid = request.getParameter("saleId");
		String product = request.getParameter("productlist");
		String client=request.getParameter("clientlist");
		String u=request.getParameter("unitprice");
		String a=request.getParameter("amount");
		String date=request.getParameter("saleDate");
		String employee=request.getParameter("employeelist");
		int amount = Integer.parseInt(a);
		int unit = Integer.parseInt(u);
		int price = amount * unit;
		if(request.getParameterValues("autoIncrease")!=null){
			String[] check = request.getParameterValues("autoIncrease");
			AutoGenerator ag = new AutoGenerator();
			sid = ag.autoIncreaseSale(list);
			System.out.println(sid+" "+ check[0]);
		}
		s.InsertSaleData(sid, client, product, amount, unit, price, date, employee);
		%>
		<script>
		location.href = "sale.jsp";   
		</script>
		<%
	}
	else{
%>
<script>
	alert('빈 값이 있습니다.');
	location.href = history.back();   
</script>
<%	
	}
%>
</body>
</html>