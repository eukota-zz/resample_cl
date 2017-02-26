; ModuleID = 'D:/Development/resample_cl/sessions/resample/resample.cl'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir-unknown-unknown"

define cc76 void @Resample(<8 x float> %coeffs, float addrspace(1)* nocapture %t, float addrspace(1)* nocapture %result) nounwind {
  %1 = tail call cc75 i32 @_Z13get_global_idj(i32 0) nounwind readnone
  %2 = getelementptr inbounds float addrspace(1)* %t, i32 %1
  %3 = load float addrspace(1)* %2, align 4, !tbaa !16
  %4 = extractelement <8 x float> %coeffs, i32 0
  %5 = tail call cc75 float @_Z3powff(float %3, float 0.000000e+00) nounwind readnone
  %6 = fmul float %4, %5
  %7 = fadd float %6, 0.000000e+00
  %8 = extractelement <8 x float> %coeffs, i32 1
  %9 = tail call cc75 float @_Z3powff(float %3, float 1.000000e+00) nounwind readnone
  %10 = fmul float %8, %9
  %11 = fadd float %7, %10
  %12 = extractelement <8 x float> %coeffs, i32 2
  %13 = tail call cc75 float @_Z3powff(float %3, float 2.000000e+00) nounwind readnone
  %14 = fmul float %12, %13
  %15 = fadd float %11, %14
  %16 = extractelement <8 x float> %coeffs, i32 3
  %17 = tail call cc75 float @_Z3powff(float %3, float 3.000000e+00) nounwind readnone
  %18 = fmul float %16, %17
  %19 = fadd float %15, %18
  %20 = extractelement <8 x float> %coeffs, i32 4
  %21 = tail call cc75 float @_Z3powff(float %3, float 4.000000e+00) nounwind readnone
  %22 = fmul float %20, %21
  %23 = fadd float %19, %22
  %24 = extractelement <8 x float> %coeffs, i32 5
  %25 = tail call cc75 float @_Z3powff(float %3, float 5.000000e+00) nounwind readnone
  %26 = fmul float %24, %25
  %27 = fadd float %23, %26
  %28 = extractelement <8 x float> %coeffs, i32 6
  %29 = tail call cc75 float @_Z3powff(float %3, float 6.000000e+00) nounwind readnone
  %30 = fmul float %28, %29
  %31 = fadd float %27, %30
  %32 = extractelement <8 x float> %coeffs, i32 7
  %33 = tail call cc75 float @_Z3powff(float %3, float 7.000000e+00) nounwind readnone
  %34 = fmul float %32, %33
  %35 = fadd float %31, %34
  %36 = getelementptr inbounds float addrspace(1)* %result, i32 %1
  store float %35, float addrspace(1)* %36, align 4, !tbaa !16
  ret void
}

declare cc75 i32 @_Z13get_global_idj(i32) nounwind readnone

declare cc75 float @_Z3powff(float, float) nounwind readnone

define cc76 void @progressiveArraySum(float addrspace(1)* nocapture %p1b_A, float addrspace(1)* nocapture %p1b_B) nounwind {
  %1 = tail call cc75 i32 @_Z13get_global_idj(i32 0) nounwind readnone
  br label %2

; <label>:2                                       ; preds = %0, %2
  %i.07 = phi i32 [ 0, %0 ], [ %6, %2 ]
  %out.06 = phi float [ 0.000000e+00, %0 ], [ %5, %2 ]
  %3 = getelementptr inbounds float addrspace(1)* %p1b_A, i32 %i.07
  %4 = load float addrspace(1)* %3, align 4, !tbaa !16
  %5 = fadd float %out.06, %4
  %6 = add i32 %i.07, 1
  %7 = icmp ugt i32 %6, %1
  br i1 %7, label %8, label %2

; <label>:8                                       ; preds = %2
  %9 = getelementptr inbounds float addrspace(1)* %p1b_B, i32 %1
  store float %5, float addrspace(1)* %9, align 4, !tbaa !16
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
