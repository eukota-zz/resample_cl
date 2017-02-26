; ModuleID = 'D:/Development/resample_cl/sessions/resample/resample.cl'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir64-unknown-unknown"

define cc76 void @Resample(<8 x float> %coeffs, float addrspace(1)* nocapture %t, float addrspace(1)* nocapture %result) nounwind {
  %1 = tail call cc75 i64 @_Z13get_global_idj(i32 0) nounwind readnone
  %2 = and i64 %1, 4294967295
  %3 = getelementptr inbounds float addrspace(1)* %t, i64 %2
  %4 = load float addrspace(1)* %3, align 4, !tbaa !16
  %5 = extractelement <8 x float> %coeffs, i32 0
  %6 = tail call cc75 float @_Z3powff(float %4, float 0.000000e+00) nounwind readnone
  %7 = fmul float %5, %6
  %8 = fadd float %7, 0.000000e+00
  %9 = extractelement <8 x float> %coeffs, i32 1
  %10 = tail call cc75 float @_Z3powff(float %4, float 1.000000e+00) nounwind readnone
  %11 = fmul float %9, %10
  %12 = fadd float %8, %11
  %13 = extractelement <8 x float> %coeffs, i32 2
  %14 = tail call cc75 float @_Z3powff(float %4, float 2.000000e+00) nounwind readnone
  %15 = fmul float %13, %14
  %16 = fadd float %12, %15
  %17 = extractelement <8 x float> %coeffs, i32 3
  %18 = tail call cc75 float @_Z3powff(float %4, float 3.000000e+00) nounwind readnone
  %19 = fmul float %17, %18
  %20 = fadd float %16, %19
  %21 = extractelement <8 x float> %coeffs, i32 4
  %22 = tail call cc75 float @_Z3powff(float %4, float 4.000000e+00) nounwind readnone
  %23 = fmul float %21, %22
  %24 = fadd float %20, %23
  %25 = extractelement <8 x float> %coeffs, i32 5
  %26 = tail call cc75 float @_Z3powff(float %4, float 5.000000e+00) nounwind readnone
  %27 = fmul float %25, %26
  %28 = fadd float %24, %27
  %29 = extractelement <8 x float> %coeffs, i32 6
  %30 = tail call cc75 float @_Z3powff(float %4, float 6.000000e+00) nounwind readnone
  %31 = fmul float %29, %30
  %32 = fadd float %28, %31
  %33 = extractelement <8 x float> %coeffs, i32 7
  %34 = tail call cc75 float @_Z3powff(float %4, float 7.000000e+00) nounwind readnone
  %35 = fmul float %33, %34
  %36 = fadd float %32, %35
  %37 = getelementptr inbounds float addrspace(1)* %result, i64 %2
  store float %36, float addrspace(1)* %37, align 4, !tbaa !16
  ret void
}

declare cc75 i64 @_Z13get_global_idj(i32) nounwind readnone

declare cc75 float @_Z3powff(float, float) nounwind readnone

define cc76 void @progressiveArraySum(float addrspace(1)* nocapture %p1b_A, float addrspace(1)* nocapture %p1b_B) nounwind {
  %1 = tail call cc75 i64 @_Z13get_global_idj(i32 0) nounwind readnone
  %2 = trunc i64 %1 to i32
  br label %3

; <label>:3                                       ; preds = %0, %3
  %i.07 = phi i32 [ 0, %0 ], [ %8, %3 ]
  %out.06 = phi float [ 0.000000e+00, %0 ], [ %7, %3 ]
  %4 = zext i32 %i.07 to i64
  %5 = getelementptr inbounds float addrspace(1)* %p1b_A, i64 %4
  %6 = load float addrspace(1)* %5, align 4, !tbaa !16
  %7 = fadd float %out.06, %6
  %8 = add i32 %i.07, 1
  %9 = icmp ugt i32 %8, %2
  br i1 %9, label %10, label %3

; <label>:10                                      ; preds = %3
  %11 = and i64 %1, 4294967295
  %12 = getelementptr inbounds float addrspace(1)* %p1b_B, i64 %11
  store float %7, float addrspace(1)* %12, align 4, !tbaa !16
  ret void
}

!opencl.kernels = !{!0, !7}
!opencl.enable.FP_CONTRACT = !{}
!opencl.spir.version = !{!14}
!opencl.ocl.version = !{!14}
!opencl.used.extensions = !{!15}
!opencl.used.optional.core.features = !{!15}
!opencl.compiler.options = !{!15}

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
!14 = metadata !{i32 1, i32 2}
!15 = metadata !{}
!16 = metadata !{metadata !"float", metadata !17}
!17 = metadata !{metadata !"omnipotent char", metadata !18}
!18 = metadata !{metadata !"Simple C/C++ TBAA"}
