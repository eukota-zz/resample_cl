; ModuleID = 'C:\Users\eukot\AppData\Local\Temp\9e784a12-7a28-47a5-8a4e-5ae03c569657.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f16:16:16-f32:32:32-f64:64:64-f80:128:128-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024-f80:128:128-n8:16:32:64"
target triple = "igil_64_GEN8"

define void @PolyEvalOcl(float addrspace(1)* %coeffs, i32 %order, float addrspace(1)* %input, float addrspace(1)* %result) {
  %1 = alloca float addrspace(1)*, align 8
  %2 = alloca i32, align 4
  %3 = alloca float addrspace(1)*, align 8
  %4 = alloca float addrspace(1)*, align 8
  %id = alloca i32, align 4
  %val = alloca float, align 4
  %out = alloca float, align 4
  %i = alloca i32, align 4
  store float addrspace(1)* %coeffs, float addrspace(1)** %1, align 8
  store i32 %order, i32* %2, align 4
  store float addrspace(1)* %input, float addrspace(1)** %3, align 8
  store float addrspace(1)* %result, float addrspace(1)** %4, align 8
  %5 = call i64 @_Z13get_global_idj(i32 0)
  %6 = trunc i64 %5 to i32
  store i32 %6, i32* %id, align 4
  %7 = load i32* %id, align 4
  %8 = zext i32 %7 to i64
  %9 = load float addrspace(1)** %3, align 8
  %10 = getelementptr inbounds float addrspace(1)* %9, i64 %8
  %11 = load float addrspace(1)* %10, align 4
  store float %11, float* %val, align 4
  store float 0.000000e+00, float* %out, align 4
  store i32 0, i32* %i, align 4
  br label %12

; <label>:12                                      ; preds = %29, %0
  %13 = load i32* %i, align 4
  %14 = load i32* %2, align 4
  %15 = icmp ule i32 %13, %14
  br i1 %15, label %16, label %32

; <label>:16                                      ; preds = %12
  %17 = load i32* %i, align 4
  %18 = zext i32 %17 to i64
  %19 = load float addrspace(1)** %1, align 8
  %20 = getelementptr inbounds float addrspace(1)* %19, i64 %18
  %21 = load float addrspace(1)* %20, align 4
  %22 = load float* %val, align 4
  %23 = load i32* %i, align 4
  %24 = uitofp i32 %23 to float
  %25 = call float @_Z3powff(float %22, float %24)
  %26 = fmul float %21, %25
  %27 = load float* %out, align 4
  %28 = fadd float %27, %26
  store float %28, float* %out, align 4
  br label %29

; <label>:29                                      ; preds = %16
  %30 = load i32* %i, align 4
  %31 = add i32 %30, 1
  store i32 %31, i32* %i, align 4
  br label %12

; <label>:32                                      ; preds = %12
  %33 = load float* %out, align 4
  %34 = load i32* %id, align 4
  %35 = zext i32 %34 to i64
  %36 = load float addrspace(1)** %4, align 8
  %37 = getelementptr inbounds float addrspace(1)* %36, i64 %35
  store float %33, float addrspace(1)* %37, align 4
  ret void
}

declare i64 @_Z13get_global_idj(i32)

declare float @_Z3powff(float, float)

define void @MatrixMultiplier(float addrspace(1)* %matrixA, float addrspace(1)* %matrixB, float addrspace(1)* %matrixC, i32 %widthA, i32 %widthB) {
  %1 = alloca float addrspace(1)*, align 8
  %2 = alloca float addrspace(1)*, align 8
  %3 = alloca float addrspace(1)*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %rowC = alloca i32, align 4
  %colC = alloca i32, align 4
  %result = alloca float, align 4
  %inner = alloca i32, align 4
  store float addrspace(1)* %matrixA, float addrspace(1)** %1, align 8
  store float addrspace(1)* %matrixB, float addrspace(1)** %2, align 8
  store float addrspace(1)* %matrixC, float addrspace(1)** %3, align 8
  store i32 %widthA, i32* %4, align 4
  store i32 %widthB, i32* %5, align 4
  %6 = call i64 @_Z13get_global_idj(i32 0)
  %7 = trunc i64 %6 to i32
  store i32 %7, i32* %rowC, align 4
  %8 = call i64 @_Z13get_global_idj(i32 1)
  %9 = trunc i64 %8 to i32
  store i32 %9, i32* %colC, align 4
  store float 0.000000e+00, float* %result, align 4
  store i32 0, i32* %inner, align 4
  br label %10

; <label>:10                                      ; preds = %36, %0
  %11 = load i32* %inner, align 4
  %12 = load i32* %4, align 4
  %13 = icmp ult i32 %11, %12
  br i1 %13, label %14, label %39

; <label>:14                                      ; preds = %10
  %15 = load i32* %rowC, align 4
  %16 = load i32* %4, align 4
  %17 = mul i32 %15, %16
  %18 = load i32* %inner, align 4
  %19 = add i32 %17, %18
  %20 = zext i32 %19 to i64
  %21 = load float addrspace(1)** %1, align 8
  %22 = getelementptr inbounds float addrspace(1)* %21, i64 %20
  %23 = load float addrspace(1)* %22, align 4
  %24 = load i32* %inner, align 4
  %25 = load i32* %5, align 4
  %26 = mul i32 %24, %25
  %27 = load i32* %colC, align 4
  %28 = add i32 %26, %27
  %29 = zext i32 %28 to i64
  %30 = load float addrspace(1)** %2, align 8
  %31 = getelementptr inbounds float addrspace(1)* %30, i64 %29
  %32 = load float addrspace(1)* %31, align 4
  %33 = fmul float %23, %32
  %34 = load float* %result, align 4
  %35 = fadd float %34, %33
  store float %35, float* %result, align 4
  br label %36

; <label>:36                                      ; preds = %14
  %37 = load i32* %inner, align 4
  %38 = add i32 %37, 1
  store i32 %38, i32* %inner, align 4
  br label %10

; <label>:39                                      ; preds = %10
  %40 = load float* %result, align 4
  %41 = load i32* %rowC, align 4
  %42 = load i32* %5, align 4
  %43 = mul i32 %41, %42
  %44 = load i32* %colC, align 4
  %45 = add i32 %43, %44
  %46 = zext i32 %45 to i64
  %47 = load float addrspace(1)** %3, align 8
  %48 = getelementptr inbounds float addrspace(1)* %47, i64 %46
  store float %40, float addrspace(1)* %48, align 4
  ret void
}

!opencl.kernels = !{!0, !7}
!opencl.compiler.options = !{!14}
!opencl.enable.FP_CONTRACT = !{}

!0 = metadata !{void (float addrspace(1)*, i32, float addrspace(1)*, float addrspace(1)*)* @PolyEvalOcl, metadata !1, metadata !2, metadata !3, metadata !4, metadata !5, metadata !6}
!1 = metadata !{metadata !"kernel_arg_addr_space", i32 1, i32 0, i32 1, i32 1}
!2 = metadata !{metadata !"kernel_arg_access_qual", metadata !"none", metadata !"none", metadata !"none", metadata !"none"}
!3 = metadata !{metadata !"kernel_arg_type", metadata !"float*", metadata !"uint", metadata !"float*", metadata !"float*"}
!4 = metadata !{metadata !"kernel_arg_type_qual", metadata !"", metadata !"", metadata !"", metadata !""}
!5 = metadata !{metadata !"kernel_arg_base_type", metadata !"float*", metadata !"uint", metadata !"float*", metadata !"float*"}
!6 = metadata !{metadata !"kernel_arg_name", metadata !"coeffs", metadata !"order", metadata !"input", metadata !"result"}
!7 = metadata !{void (float addrspace(1)*, float addrspace(1)*, float addrspace(1)*, i32, i32)* @MatrixMultiplier, metadata !8, metadata !9, metadata !10, metadata !11, metadata !12, metadata !13}
!8 = metadata !{metadata !"kernel_arg_addr_space", i32 1, i32 1, i32 1, i32 0, i32 0}
!9 = metadata !{metadata !"kernel_arg_access_qual", metadata !"none", metadata !"none", metadata !"none", metadata !"none", metadata !"none"}
!10 = metadata !{metadata !"kernel_arg_type", metadata !"float*", metadata !"float*", metadata !"float*", metadata !"uint", metadata !"uint"}
!11 = metadata !{metadata !"kernel_arg_type_qual", metadata !"", metadata !"", metadata !"", metadata !"", metadata !""}
!12 = metadata !{metadata !"kernel_arg_base_type", metadata !"float*", metadata !"float*", metadata !"float*", metadata !"uint", metadata !"uint"}
!13 = metadata !{metadata !"kernel_arg_name", metadata !"matrixA", metadata !"matrixB", metadata !"matrixC", metadata !"widthA", metadata !"widthB"}
!14 = metadata !{metadata !"-cl-std=CL1.2", metadata !"-cl-kernel-arg-info"}
