<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@page import="com.db.*" %>
<%@page import="java.sql.*" %>
<%@page import="java.util.ArrayList" %>
<!DOCTYPE html>
<html>
<script>
    function idDelete(delID){
       
        //alert(delID);
        location.href = "client.jsp?delid=" + delID;   //get방식으로 삭제할아이디를 넘김
        //var delid=delID;
 
    }
    function idUpdate(upID){
    	location.href="clientUpdate.jsp?upid=" + upID;
    }
</script>
<%
	SemiERPDAO s = new SemiERPDAO();
	s.dbConn();
	request.setCharacterEncoding("utf-8");
	if(request.getParameter("clientId")!=null && request.getParameter("clientName")!=null && request.getParameter("address")!=null
			&& request.getParameter("mail")!=null && request.getParameter("phoneNum")!=null){
		String id = request.getParameter("clientId");
		String name = request.getParameter("clientName");
		String addr = request.getParameter("address");
		String mail = request.getParameter("mail");
		String phone = request.getParameter("phoneNum");
		//int phone = Integer.parseInt(p);
		s.InsertAccountListtData(id, name, addr, mail, phone);
	}
	else{
%>
<!-- 
<script>
	alert('빈 값이 있습니다.');
</script>
-->
<%	
	}
	boolean delresult = s.DeleteAccountListtData(request.getParameter("delid"));
%>
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

<body>
<div class="container-fluid">
	<!-- 상단메뉴 영역 시작 -->
	<div class="row" id="row1">
		<div class="col-md-12">
			<ul class="nav">
				<li class="nav-item">
					<a class="nav-link active" href="main.jsp"><h3>ERP System</h3></a>
				</li>
				<li class="nav-item">
					<a class="nav-link" href="#">프로필</a>
				</li>
				<li class="nav-item">
					<a class="nav-link disabled" href="#">메세지</a>
				</li>
				<li class="nav-item dropdown ml-md-auto">
					 <a class="nav-link dropdown-toggle" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">회원정보</a>
					<div class="dropdown-menu dropdown-menu-right" aria-labelledby="navbarDropdownMenuLink">
						 <a class="dropdown-item" href="index.jsp">로그인</a>
						 <a class="dropdown-item" href="userLogout.jsp">로그아웃</a>
						<div class="dropdown-divider">
						</div> <a class="dropdown-item" href="#">회원정보수정</a>
					</div>
				</li>
			</ul>
		</div>
	</div>
	<!-- 상단메뉴 영역 끝 -->
	
	<div class="row" style="font-size: 14px;" id="row2">
		<!-- 좌측메뉴 -->
		<div class="col-md-2 mt-3">	
			<div id="card-214190">
				<div class="card">
					<div class="card-header">
						 <a class="card-link collapsed" data-toggle="collapse" data-parent="#card-214190" href="#card-element-1">자재구매</a>
					</div>
					<div id="card-element-1" class="collapse">
						<div class="card-body">
							<ul class="nav flex-column">
								<li class="nav-item">
									<a class="nav-link active" href="materials.jsp">자재관리</a>
									<a class="nav-link active" href="purchase.jsp">구매관리</a>
								</li>
							</ul>
						</div>
					</div>
				</div>
				
				<div class="card">
					<div class="card-header">
						 <a class="card-link collapsed" data-toggle="collapse" data-parent="#card-214190" href="#card-element-2">제품생산</a>
					</div>
					<div id="card-element-2" class="collapse">
						<div class="card-body">
							<ul class="nav flex-column">
								<li class="nav-item">
									<a class="nav-link active" href="product.jsp">제품관리</a>
									<a class="nav-link active" href="produce_list.jsp">생산목록</a>
									<a class="nav-link active" href="produce.jsp">생산관리</a>
								</li>
							</ul>
						</div>
					</div>
				</div>
				
				<div class="card">
					<div class="card-header">
						 <a class="card-link collapsed" data-toggle="collapse" data-parent="#card-214190" href="#card-element-3">판매관리</a>
					</div>
					<div id="card-element-3" class="collapse">
						<div class="card-body">
							<ul class="nav flex-column">
								<li class="nav-item">
									<a class="nav-link active" href="sale.jsp">판매관리</a>
								</li>
							</ul>
						</div>
					</div>
				</div>
				
				<div class="card">
					<div class="card-header">
						 <a class="card-link collapsed" data-toggle="collapse" data-parent="#card-214190" href="#card-element-4">회사관리</a>
					</div>
					<div id="card-element-4" class="collapse">
						<div class="card-body">
							<ul class="nav flex-column">
								<li class="nav-item">
									<a class="nav-link active" href="client.jsp">거래처관리</a>
									<a class="nav-link active" href="employee.jsp">사원관리</a>
							</ul>
						</div>
					</div>
				</div>
			</div>
		</div>
		<!-- 좌측메뉴 끝 -->
		<!-- 거래처관리 폼 시작 -->
		<div class="col-md-3 mt-6" style="font-size: 13px;">
			<form method='post'>
			<br>
				<h2>거래처 관리</h2>
				<hr>
			  	  <div class="form-row">
				  	  <div class="form-group col-sm-8">
				    	<label for="clientId">거래처 코드</label>
				    	<input type="number" min="0" name="clientId" class="form-control form-control-sm" id="clientId" placeholder="거래처ID">
				    </div>
				    <div class="form-group col-sm-4">
				    	<br>
					    <div class="checkbox">
				    		<label>
				      		<input type="checkbox" id="autoIncrease">자동 생성
				    		</label>
				  	  	</div>
				  	</div>
				  </div>
				  
				  <div class="form-row">
				    <label for="clientName">회사명</label>
				    <input type="text" name="clientName" class="form-control form-control-sm" id="clientName" placeholder="회사명">
				  </div><br>
				  
				  <div class="form-row">
				    	<label for="address">주소</label>
				    	<input type="text" name="address" class="form-control form-control-sm" id="address" placeholder="주소">
				  </div><br>
				  
				  <div class="form-row">
				   	<label>메일</label>
				    <input type="email" name="mail" class="form-control form-control-sm" id="mail" placeholder="메일">
				  </div><br>
				  
				  <div class="form-row">
				   	<label>전화번호</label>
				    <input type="text" name="phoneNum" class="form-control form-control-sm" id="phoneNum" placeholder="전화번호">
				  </div><br>
				  <hr>
				  <div class="form-row">
				  	<div class="form-group col-sm-9"></div>
				  	<div class="form-group col-sm-3">
				  		<button type="submit" class="btn btn-primary btn-sm">거래처 등록</button>
				  	</div>
				  </div>
				  
			</form>
		</div>
		<!-- 거래처관리 폼 끝 -->
		
		<!-- 검색/열람영역 -->
		<div class="col-md-6 mt-6" style="font-size: 13px;">
			<!-- 검색옵션 -->
			<h3>검색</h3>
			<div class="form-group">
				<label for="clientName">회사명</label>
		    	<input type="text" name="clientName" class="form-control form-control-sm" id="clientName" placeholder="회사명">
			</div>	
			<div class="form-group">
				<button type="submit" class="btn btn-primary btn-sm">검색</button>
			</div>
			
			<hr>
			<!-- 검색옵션끝 -->
			<!-- 데이터 열람테이블 -->
			<table class="table table-hover">
			  <thead>
			    <tr align="center">
			      <th scope="col">No.<br></th>
			      <th scope="col">거래처코드<br></th>
			      <th scope="col">회사명<br></th>
			      <th scope="col">주소<br></th>
			      <th scope="col">메일<br></th>
			      <th scope="col">전화번호<br></th>
			      <th scope="col"> <br></th>
			      <th scope="col"> <br></th>
			    </tr>
			  </thead>
			  <tbody align="center">
			    
			 <%
			ArrayList<AccountListDTO> list=s.AccountListList();
			int i=1;
			for(AccountListDTO dto:list){
			 %>
			    <tr align="center">
			      <td><%=i++ %></td>
			      <td><%=dto.getACCOUNT_BID() %></td>
			      <td><%=dto.getBUSINESS_NAME() %></td>
			      <td><%=dto.getADDRESS() %></td>
			      <td><%=dto.getEMAIL() %></td>
			      <td><%=dto.getPHONE_NUMBER() %></td>
			      <td><button class="btn btn-default btn-sm" name="button" onclick="idDelete('<%=dto.getACCOUNT_BID() %>')">삭제</button></td>
			      <td><button class="btn btn-default btn-sm" name="button" onclick="idUpdate('<%=dto.getACCOUNT_BID() %>')">수정</button></td>
			    </tr>
			 <% }
			 %>
			  </tbody>
			</table> 
			<!-- 데이터 열람테이블 끝 -->
		</div>
		<!-- 검색/열람영역끝 -->
		
	</div>
</div>  


	<footer class="bg-dark mt-4 p-5 text-center" style="color: #FFFFFF;">
		Copyright &copy; 세미콜론 All Rights Reserved.
	</footer>
	<!-- 제이쿼리 자바스크립트 추가하기 -->
	<script src="./js/jquery.min.js"></script>
	<!-- 파퍼 자바스크립트 추가하기 -->
	<script src="./js/popper.js"></script>
	<!-- 부트스트랩 자바스크립트 추가하기 -->
	<script src="./js/bootstrap.min.js"></script>
</body>
</html>