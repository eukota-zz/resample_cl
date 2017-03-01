; ModuleID = 'C:\Users\eukot\AppData\Local\Temp\da7cecf3-9550-4de1-b02c-57939d2b0b1c.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f16:16:16-f32:32:32-f64:64:64-f80:128:128-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024-f80:128:128-n8:16:32:64"
target triple = "igil_64_GEN8"

define void @Resample(<8 x float> %coeffs, float addrspace(1)* %t, float addrspace(1)* %result) {
  %1 = alloca <8 x float>, align 32
  %2 = alloca float addrspace(1)*, align 8
  %3 = alloca float addrspace(1)*, align 8
  %id = alloca i32, align 4
  %val = alloca float, align 4
  %out = alloca float, align 4
  %i = alloca i32, align 4
  store <8 x float> %coeffs, <8 x float>* %1, align 32
  store float addrspace(1)* %t, float addrspace(1)** %2, align 8
  store float addrspace(1)* %result, float addrspace(1)** %3, align 8
  %4 = call i64 @_Z13get_global_idj(i32 0)
  %5 = trunc i64 %4 to i32
  store i32 %5, i32* %id, align 4
  %6 = load i32* %id, align 4
  %7 = zext i32 %6 to i64
  %8 = load float addrspace(1)** %2, align 8
  %9 = getelementptr inbounds float addrspace(1)* %8, i64 %7
  %10 = load float addrspace(1)* %9, align 4
  store float %10, float* %val, align 4
  store float 0.000000e+00, float* %out, align 4
  store i32 0, i32* %i, align 4
  br label %11

; <label>:11                                      ; preds = %25, %0
  %12 = load i32* %i, align 4
  %13 = icmp ult i32 %12, 8
  br i1 %13, label %14, label %28

; <label>:14                                      ; preds = %11
  %15 = load <8 x float>* %1, align 32
  %16 = load i32* %i, align 4
  %17 = extractelement <8 x float> %15, i32 %16
  %18 = load float* %val, align 4
  %19 = load i32* %i, align 4
  %20 = uitofp i32 %19 to float
  %21 = call float @_Z3powff(float %18, float %20)
  %22 = fmul float %17, %21
  %23 = load float* %out, align 4
  %24 = fadd float %23, %22
  store float %24, float* %out, align 4
  br label %25

; <label>:25                                      ; preds = %14
  %26 = load i32* %i, align 4
  %27 = add i32 %26, 1
  store i32 %27, i32* %i, align 4
  br label %11

; <label>:28                                      ; preds = %11
  %29 = load float* %out, align 4
  %30 = load i32* %id, align 4
  %31 = zext i32 %30 to i64
  %32 = load float addrspace(1)** %3, align 8
  %33 = getelementptr inbounds float addrspace(1)* %32, i64 %31
  store float %29, float addrspace(1)* %33, align 4
  ret void
}

declare i64 @_Z13get_global_idj(i32)

declare float @_Z3powff(float, float)

define void @progressiveArraySum(float addrspace(1)* %p1b_A, float addrspace(1)* %p1b_B) {
  %1 = alloca float addrspace(1)*, align 8
  %2 = alloca float addrspace(1)*, align 8
  %id = alloca i32, align 4
  %out = alloca float, align 4
  %i = alloca i32, align 4
  store float addrspace(1)* %p1b_A, float addrspace(1)** %1, align 8
  store float addrspace(1)* %p1b_B, float addrspace(1)** %2, align 8
  %3 = call i64 @_Z13get_global_idj(i32 0)
  %4 = trunc i64 %3 to i32
  store i32 %4, i32* %id, align 4
  store float 0.000000e+00, float* %out, align 4
  store i32 0, i32* %i, align 4
  br label %5

; <label>:5                                       ; preds = %17, %0
  %6 = load i32* %i, align 4
  %7 = load i32* %id, align 4
  %8 = icmp ule i32 %6, %7
  br i1 %8, label %9, label %20

; <label>:9                                       ; preds = %5
  %10 = load i32* %i, align 4
  %11 = zext i32 %10 to i64
  %12 = load float addrspace(1)** %1, align 8
  %13 = getelementptr inbounds float addrspace(1)* %12, i64 %11
  %14 = load float addrspace(1)* %13, align 4
  %15 = load float* %out, align 4
  %16 = fadd float %15, %14
  store float %16, float* %out, align 4
  br label %17

; <label>:17                                      ; preds = %9
  %18 = load i32* %i, align 4
  %19 = add i32 %18, 1
  store i32 %19, i32* %i, align 4
  br label %5

; <label>:20                                      ; preds = %5
  %21 = load float* %out, align 4
  %22 = load i32* %id, align 4
  %23 = zext i32 %22 to i64
  %24 = load float addrspace(1)** %2, align 8
  %25 = getelementptr inbounds float addrspace(1)* %24, i64 %23
  store float %21, float addrspace(1)* %25, align 4
  ret void
}

!opencl.kernels = !{!0, !7}
!opencl.compiler.options = !{!14}
!opencl.enable.FP_CONTRACT = !{}

!0 = metadata !{void (<8 x float>, float addrspace(1)*, float addrspace(1)*)* @Resample, metadata !1, metadata !2, metadata !3, metadata !4, metadata !5, metadata !6}
!1 = metadata !{metadata !"kernel_arg_addr_space", i32 0, i32 1, i32 1}
!2 = metadata !{metadata !"kernel_arg_access_qual", metadata !"none", metadata !"none", metadata !"none"}
!3 = metadata !{metadata !"kernel_arg_type", metadata !"float8", metadata !"float*", metadata !"float*"}
!4 = metadata !{metadata !"kernel_arg_type_qual", metadata !"", metadata !"", metadata !""}
!5 = metadata !{metadata !"kernel_arg_base_type", metadata !"float8", metadata !"float*", metadata !"float*"}
!6 = metadata !{metadata !"kernel_arg_name", metadata !"coeffs", metadata !"t", metadata !"result"}
!7 = metadata !{void (float addrspace(1)*, float addrspace(1)*)* @progressiveArraySum, metadata !8, metadata !9, metadata !10, metadata !11, metadata !12, metadata !13}
!8 = metadata !{metadata !"kernel_arg_addr_space", i32 1, i32 1}
!9 = metadata !{metadata !"kernel_arg_access_qual", metadata !"none", metadata !"none"}
!10 = metadata !{metadata !"kernel_arg_type", metadata !"float*", metadata !"float*"}
!11 = metadata !{metadata !"kernel_arg_type_qual", metadata !"", metadata !""}
!12 = metadata !{metadata !"kernel_arg_base_type", metadata !"float*", metadata !"float*"}
!13 = metadata !{metadata !"kernel_arg_name", metadata !"p1b_A", metadata !"p1b_B"}
!14 = metadata !{metadata !"-cl-std=CL1.2", metadata !"-cl-kernel-arg-info"}
