<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@page import="com.db.*" %>
<%@page import="java.sql.*" %>
<%@page import="java.util.ArrayList" %>
<!DOCTYPE html>
<html>
<style>
.dropdown:hover .dropdown-menu {
    display: block;
    margin-top: 0;
}
</style>
<script>
document.forms['form1'].reset();
	function idDelete(delID){
	    //alert(delID);
	    location.href = "delete.jsp?delid=" + delID + "&page=employee";   //get방식으로 삭제할아이디를 넘김
	}
		
    function idUpdate(upID){
    	location.href="employeeUpdate.jsp?upid=" + upID;
    }
    
</script>
<%
if(session.getAttribute("loginUser")==null){ %>
<script>
    alert("로그인 하세요.");
    location.href="index.jsp";
</script>
<%
}
%>
<%
	SemiERPDAO s = new SemiERPDAO();
	s.dbConn();
	request.setCharacterEncoding("utf-8");
	ArrayList<EmployeeDTO> list=s.EmployeeList();
	EmployeeDTO update = new EmployeeDTO();
	for(EmployeeDTO dto:list){
		if(dto.getEID().equals(request.getParameter("upid"))){
			update=dto;
			break;
		}
	}
	String id="0";
	if(request.getParameter("personalId")!=null && request.getParameter("password")!=null && request.getParameter("department")!=null
			&& request.getParameter("rank")!=null && request.getParameter("phoneNum")!=null && request.getParameter("name")!=null
			&& request.getParameter("joinDate")!=null && request.getParameter("payNum")!=null){
		//id = request.getParameter("personalId");
		String name = request.getParameter("name");
		String pw = request.getParameter("password");
		String dep = request.getParameter("department");
		String rank = request.getParameter("rank");
		String date = request.getParameter("joinDate");
		String n = request.getParameter("payNum");
		String phone = request.getParameter("phoneNum");
		int pay = Integer.parseInt(n);
		
		if(request.getParameterValues("randomPassword")!=null){
			AutoGenerator ag = new AutoGenerator();
			pw = ag.autoPassword();
		}
		//String[] check = request.getParameterValues("autoIncrease");
		if(request.getParameterValues("autoIncrease")!=null){
			String[] check = request.getParameterValues("autoIncrease");
			AutoGenerator ag = new AutoGenerator();
			id = ag.autoIncreaseEmployee(list);
			System.out.println(id+" "+ check[0]);
		}
		else{
			if(request.getParameter("personalId")!=""){
				id = request.getParameter("personalId");
			}
		}
		s.UpdateEmployeeData(id, name, pw, dep, rank, phone, pay, date);
		%>
		<script>
		location.href = "employee.jsp";   
		</script>
		<%
	}
%>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
	<title>Enterprise Resource Planning System</title>
	<!-- 부트스트랩 CSS 추가하기 -->
	<link rel="stylesheet" href="./css/bootstrap.min.css">
	<!-- 커스텀 CSS 추가하기 -->
	<link rel="stylesheet" href="./css/custom.css">
	<link rel="stylesheet" href="./css/footable.bootstrap.css">
	<link rel="stylesheet" href="./css/footable.bootstrap.min.css">
	<!-- 
     <link rel="stylesheet" href="https://cdn.datatables.net/t/bs-3.3.6/jqc-1.12.0,dt-1.10.11/datatables.min.css"/> 
	-->
	
	<!-- 테이블 검색 스크립트 --> 
	<script src="https://cdn.datatables.net/t/bs-3.3.6/jqc-1.12.0,dt-1.10.11/datatables.min.js"></script>
	    <script>
        jQuery(function($){
            //$("#foo-table").DataTable();
            $("#foo-table").DataTable({
            	// 표시 건수기능 숨기기
            	lengthChange: false,
            	// 검색 기능 숨기기
            	searching: false,
            	// 정렬 기능 숨기기
            	//ordering: false,
            	// 정보 표시 숨기기
            	info: false,
            	// 페이징 기능 숨기기
            	paging: false
            });
        });
        jQuery(function($){
        	$('.foo-table').footable();
        });
    </script>
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/js/bootstrap.min.js"></script>
	<script src="./js/footable.min.js"></script> 
	<script src="./js/footable.js"></script> 

</head>
<body>
<div class="container-fluid mt-2 mb-5">
	<div class="row">
		<div class="col-md-12">
			<nav class="navbar navbar-expand-lg navbar-light bg-light fixed-bottom" style="font-size: 17px;">
				<div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
					<ul class="navbar-nav">
						<li class="nav-item mt-2 mr-5 text-secondary">
							<label>ERP 메뉴</label>
						</li>
						<li class="nav-item mt-2 mr-2">
							<label>|</label>
						</li>
						<li class="nav-item dropup mr-2">
							 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">자재구매</a>
							<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
								 <a class="nav-link active" href="materials.jsp">자재관리</a>
								 <a class="nav-link active" href="purchase.jsp">구매관리</a>
							</div>
						</li>
						<li class="nav-item mt-2 mr-2">
							<label>|</label>
						</li>
						<li class="nav-item dropup mr-2">
							 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">제품생산</a>
							<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
								<a class="nav-link active" href="product.jsp">제품관리</a>
								<a class="nav-link active" href="produce_list.jsp">생산목록</a>
								<a class="nav-link active" href="produce.jsp">생산관리</a>
							</div>
						</li>
						<li class="nav-item mt-2 mr-2" style="color:#111111">
							<label>|</label>
						</li>
						<li class="nav-item dropup mr-2">
							 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">판매관리</a>
							<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
								<a class="nav-link active" href="sale.jsp">판매관리</a>
							</div>
						</li>
						<li class="nav-item mt-2 mr-2" style="color:#111111">
							<label>|</label>
						</li>
						<li class="nav-item dropup mr-2">
							 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">회사관리</a>
							<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
								<a class="nav-link active" href="client.jsp">거래처관리</a>
								<a class="nav-link active" href="employee.jsp">사원관리</a>
							</div>
						</li>
						<li class="nav-item mt-2 mr-2" style="color:#111111">
							<label>|</label>
						</li>
					</ul>
					<ul class="navbar-nav ml-md-auto">
						<li class="nav-item dropup">
							 <a class="nav-link dropdown-toggle text-primary" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">전체 메뉴</a>
							<div class="dropdown-menu dropdown-menu-right" aria-labelledby="navbarDropdownMenuLink">
								<a class="nav-link active" href="materials.jsp">자재관리</a>
								<a class="nav-link active" href="purchase.jsp">구매관리</a>
								<div class="dropdown-divider"></div>
								<a class="nav-link active" href="product.jsp">제품관리</a>		
								<a class="nav-link active" href="produce_list.jsp">생산목록</a>	
								<a class="nav-link active" href="produce.jsp">생산관리</a>
								<div class="dropdown-divider"></div>
								<a class="nav-link active" href="sale.jsp">판매관리</a>
								<div class="dropdown-divider"></div>
								<a class="nav-link active" href="client.jsp">거래처관리</a>
								<a class="nav-link active" href="employee.jsp">사원관리</a>
							</div>
						</li>
					</ul>
				</div>
			</nav>
			<div class="row" id="row1">
				<div class="col-md-12">
					<ul class="nav">
						<li class="nav-item">
							<a class="nav-link active" href="main.jsp"><h3>ERP System</h3></a>
						</li>
						<li class="nav-item">
							<a class="nav-link" href="profile.jsp">프로필</a>
						</li>
						<li class="nav-item dropdown ml-md-auto">
							 <a class="nav-link dropdown-toggle" href="http://example.com" id="hostinform" data-toggle="dropdown">회원정보</a>
							<div class="dropdown-menu dropdown-menu-right" aria-labelledby="hostinform">
								 <a class="dropdown-item" href="index.jsp">로그인</a>
								 <a class="dropdown-item" href="logoutAction.jsp">로그아웃</a>
								<div class="dropdown-divider">
								</div> <a class="dropdown-item" href="profile.jsp">프로필</a>
							</div>
						</li>
					</ul>
				</div>
			</div>
			<div class="row mb-1" id="row2">
			<div class="col-md-12">
				<nav class="navbar navbar-expand-lg navbar-light bg-white" style="font-size: 17px;">
					<div class="collapse navbar-collapse" id="bs-example-navbar-collapse-2">
						<ul class="navbar-nav">
							<li class="nav-item mt-2 mr-5 text-secondary">
								<label>ERP 메뉴</label>
							</li>
							<li class="nav-item mt-2 mr-3">
								<label>|</label>
							</li>
							<li class="nav-item dropdown mr-2">
								 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">자재구매</a>
								<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
									 <a class="nav-link active" href="materials.jsp">자재관리</a>
									 <a class="nav-link active" href="purchase.jsp">구매관리</a>
								</div>
							</li>
							<li class="nav-item mt-2 mr-3">
								<label>|</label>
							</li>
							<li class="nav-item dropdown mr-3">
								 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">제품생산</a>
								<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
									<a class="nav-link active" href="product.jsp">제품관리</a>
									<a class="nav-link active" href="produce_list.jsp">생산목록</a>
									<a class="nav-link active" href="produce.jsp">생산관리</a>
								</div>
							</li>
							<li class="nav-item mt-2 mr-3" style="color:#111111">
								<label>|</label>
							</li>
							<li class="nav-item dropdown mr-3">
								 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">판매관리</a>
								<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
									<a class="nav-link active" href="sale.jsp">판매관리</a>
								</div>
							</li>
							<li class="nav-item mt-2 mr-3" style="color:#111111">
								<label>|</label>
							</li>
							<li class="nav-item dropdown mr-3">
								 <a class="nav-link dropdown-toggle text-dark" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">회사관리</a>
								<div class="dropdown-menu" aria-labelledby="navbarDropdownMenuLink">
									<a class="nav-link active" href="client.jsp">거래처관리</a>
									<a class="nav-link active" href="employee.jsp">사원관리</a>
								</div>
							</li>
							<li class="nav-item mt-2 mr-3" style="color:#111111">
								<label>|</label>
							</li>
						</ul>
						<ul class="navbar-nav ml-md-auto">
							<li class="nav-item dropdown">
								 <a class="nav-link dropdown-toggle text-primary" href="http://example.com" id="navbarDropdownMenuLink" data-toggle="dropdown">전체 메뉴</a>
								<div class="dropdown-menu dropdown-menu-right" aria-labelledby="navbarDropdownMenuLink">
									<a class="nav-link active" href="materials.jsp">자재관리</a>
									<a class="nav-link active" href="purchase.jsp">구매관리</a>
									<div class="dropdown-divider"></div>
									<a class="nav-link active" href="product.jsp">제품관리</a>		
									<a class="nav-link active" href="produce_list.jsp">생산목록</a>	
									<a class="nav-link active" href="produce.jsp">생산관리</a>
									<div class="dropdown-divider"></div>
									<a class="nav-link active" href="sale.jsp">판매관리</a>
									<div class="dropdown-divider"></div>
									<a class="nav-link active" href="client.jsp">거래처관리</a>
									<a class="nav-link active" href="employee.jsp">사원관리</a>
								</div>
							</li>
						</ul>
					</div>
				</nav>
			</div>
			</div>
		</div>
	</div>
	<hr>
	<div class="row mt-4">
		<!-- 사원관리 폼 시작 -->
		<div class="col-md-2 mt-6" style="font-size: 13px;">
			<form method='post'>
			<br>
				<h2>사원 관리</h2>
				<hr>
			  	  <div class="form-row">
				  	  <div class="form-group col-sm-8">
				    	<label>사원코드</label>
				    	<input type="number" min="0" name="personalId" class="form-control form-control-sm" id="personalId" placeholder="사원ID" value=<%=update.getEID() %> readonly>
				    </div>
				    <div class="form-group col-sm-4">
				    	<br>
					    <div class="checkbox">
				    		<label>
				      		<input type="checkbox" name="autoIncrease" value="0">자동 생성
				    		</label>
				  	  	</div>
				  	</div>
				  </div>
				  
				  <div class="form-row">
				    <div class="form-group col-sm-8">
				    	<label>패스워드</label>
				    	<input type="password" name="password" class="form-control form-control-sm" id="password" placeholder="패스워드" value=<%=update.getE_PASSWORD() %>>
				    </div>
				    <div class="form-group col-sm-4">
				    	<br>
					    <div class="checkbox">
				    		<label>
				      		<input type="checkbox" name="randomPassword" value="0">랜덤 암호
				    		</label>
				  	  	</div>
				  	</div>
				  </div>
				  
				  <div class="form-row">
				    <div class="form-group col-sm-6">
				    	<label>이름</label>
				    	<input type="text" name="name" class="form-control form-control-sm" id="name" placeholder="이름" value=<%=update.getE_NAME()%>>
				    </div>
				    <div class="form-group col-sm-6">
				    	<label>호봉</label>
				    	<input type="number" min="0" max="32" name="payNum" class="form-control form-control-sm" id="payNum" placeholder="호봉" value=<%=update.getSALARY() %>>
				  	</div>
				  </div>
				  
				  <div class="form-row">
				  	<div class="form-group col-sm-6">
				    	<label>부서</label>
				    	<select name="department" class="form-control form-control-sm" id="department">
							<%if(update.getDEPARTMENT().equals("인사관리")){ %>
								<option value="인사관리" selected>인사관리</option>
								<option value="생산관리">생산관리</option>
								<option value="영업관리">영업관리</option>
							<%}else if(update.getDEPARTMENT().equals("생산관리")){ %>
								<option value="인사관리">인사관리</option>
								<option value="생산관리" selected>생산관리</option>
								<option value="영업관리">영업관리</option>
							<%}else if(update.getDEPARTMENT().equals("영업관리")){ %>
								<option value="인사관리" selected>인사관리</option>
								<option value="생산관리">생산관리</option>
								<option value="영업관리" selected>영업관리</option>
							<%}%>
						</select>
				  	</div>
				    <div class="form-group col-sm-6">
				    	<label>직급</label>
				    	<select name="rank" class="form-control form-control-sm" id="rank" value=<%=update.getE_POSITION() %>>
				    		<%if(update.getE_POSITION().equals("사원")){ %>
				    			<option value="사원" selected>사원</option>
								<option value="대리">대리</option>
								<option value="과장">과장</option>
								<option value="차장">차장</option>
								<option value="부장">부장</option>
								<option value="사장">사장</option>
				    		<%}else if(update.getE_POSITION().equals("대리")){ %>
				    			<option value="사원">사원</option>
								<option value="대리" selected>대리</option>
								<option value="과장">과장</option>
								<option value="차장">차장</option>
								<option value="부장">부장</option>
								<option value="사장">사장</option>
				    		<%}else if(update.getE_POSITION().equals("과장")){ %>
				    			<option value="사원">사원</option>
								<option value="대리">대리</option>
								<option value="과장" selected>과장</option>
								<option value="차장">차장</option>
								<option value="부장">부장</option>
								<option value="사장">사장</option>
				    		<%}else if(update.getE_POSITION().equals("차장")){ %>
				    			<option value="사원">사원</option>
								<option value="대리">대리</option>
								<option value="과장">과장</option>
								<option value="차장" selected>차장</option>
								<option value="부장">부장</option>
								<option value="사장">사장</option>
				    		<%}else if(update.getE_POSITION().equals("부장")){ %>
				    			<option value="사원">사원</option>
								<option value="대리">대리</option>
								<option value="과장">과장</option>
								<option value="차장">차장</option>
								<option value="부장" selected>부장</option>
								<option value="사장">사장</option>
				    		<%}else if(update.getE_POSITION().equals("사장")){ %>
				    			<option value="사원">사원</option>
								<option value="대리">대리</option>
								<option value="과장">과장</option>
								<option value="차장">차장</option>
								<option value="부장">부장</option>
								<option value="사장" selected>사장</option>
				    		<%} %>
						</select>
				    </div>
				  </div>
				  				  
				  <div class="form-row">
				    <label>전화번호</label>
				    <input type="text" name="phoneNum" class="form-control form-control-sm" id="phoneNum" placeholder="전화번호" value=<%=update.getPHONE_NUMBER() %>>
				  </div><br>
				  <div class="form-row">
				    	<label>입사일자</label>
				    	<input type="date" name="joinDate" class="form-control form-control-sm" id="joinDate" value=<%=update.getJOIN_DATE() %>>
				  </div>
				  
				  <hr>
				  <div class="form-row">
				  	<div class="form-group col-sm-9"></div>
				  	<div class="form-group col-sm-3">
				  		<button type="submit" class="btn btn-primary btn-sm">사원 수정</button>
				  	</div>
				  </div>
				  
			</form>
		</div>
		<!-- 사원관리 폼 끝 -->
		<div class="col-md-10">
			<!-- 검색옵션 -->
			<form action='employeesearch.jsp'>
			<h3>검색</h3>
			<div class="form-row">
				<div class="form-group col-sm-4">
					<label>이름</label>
					<input type="text" name="sName" class="form-control form-control-sm" id="sName" placeholder="이름">    
				</div>
				<div class="form-group col-sm-4">
					<label>부서</label>
					    <select name="sDepartment" class="form-control form-control-sm" id="sDepartment">
							<option value="">-</option>
							<option value="인사관리">인사관리</option>
							<option value="생산관리">생산관리</option>
							<option value="영업관리">영업관리</option>
						</select>
				</div>
				<div class="form-group col-sm-4">
				    	<label>직급</label>
				    	<select name="sRank" class="form-control form-control-sm" id="sRank">
				    				<option value="">-</option>
									<option value="사원">사원</option>
									<option value="대리">대리</option>
									<option value="과장">과장</option>
									<option value="차장">차장</option>
									<option value="부장">부장</option>
									<option value="사장">사장</option>
						</select>
				    </div>
			</div>
			<label>입사일자</label>
			<div class="form-row">
				<div class="form-group col-sm-5">
					<input type="date" name="startDate" class="form-control form-control-sm" id="startDate">
				</div>	
				<div class="form-group col-sm-2" style="text-align:center">
					<p>~</p>
				</div>		
				<div class="form-group col-sm-5">
					<input type="date" name="endDate" class="form-control form-control-sm" id="endDate">
				</div>			
			</div>
			<div class="form-group">
				<button type="submit" class="btn btn-primary btn-sm">검색</button>
			</div>
			
			<hr>
			</form>
			<!-- 검색옵션끝 -->
			<!-- 데이터 열람테이블 -->
			<table id="foo-table" class="table table-hover" style="width:1200px; font-size:12px;">
			  <thead>
			    <tr align="center">
			      <th scope="col">No.<br></th>
			      <th scope="col">사원ID<br></th>
			      <th scope="col">사원명<br></th>
			      <th scope="col">패스워드<br></th>
			      <th scope="col">부서<br></th>
			      <th scope="col">직급<br></th>
			      <th scope="col">전화번호<br></th>
			      <th scope="col">호봉<br></th>
			      <th scope="col">입사일<br></th>
			      <th scope="col"> <br></th>
			      <th scope="col"> <br></th>
			    </tr>
			  </thead>
			  <tbody align="center">
			    
			 <%
			int i=1;
			for(EmployeeDTO dto:list){
			 %>
			    <tr align="center">
			      <td><%=i++ %></td>
			      <td><%=dto.getEID() %></td>
			      <td><%=dto.getE_NAME() %></td>
			      <td><%=dto.getE_PASSWORD() %></td>
			      <td><%=dto.getDEPARTMENT() %></td>
			      <td><%=dto.getE_POSITION() %></td>
			      <td><%=dto.getPHONE_NUMBER() %></td>
			      <td><%=dto.getSALARY() %></td>
			      <td><%=dto.getJOIN_DATE() %></td>
			      <td><button class="btn btn-default btn-sm" name="button" onclick="idDelete('<%=dto.getEID() %>')">삭제</button></td>
			      <td><button class="btn btn-default btn-sm" name="button" onclick="idUpdate('<%=dto.getEID() %>')">수정</button></td>
			    </tr>
			 <% }
			 %>
			  </tbody>
			</table> 
			<!-- 데이터 열람테이블 끝 -->
		</div>
	</div>
</div>
	<!-- 
	<footer class="bg-dark mt-4 p-5 text-center" style="color: #FFFFFF;">
		Copyright &copy; 세미콜론 All Rights Reserved.
	</footer>
	 -->
	<!-- 제이쿼리 자바스크립트 추가하기 -->
	<script src="./js/jquery.min.js"></script>
	<!-- 파퍼 자바스크립트 추가하기 -->
	<script src="./js/popper.js"></script>
	<!-- 부트스트랩 자바스크립트 추가하기 -->
	<script src="./js/bootstrap.min.js"></script>
</body>
</html>