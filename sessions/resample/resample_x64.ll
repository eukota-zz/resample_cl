; ModuleID = 'D:/Development/resample_cl/sessions/resample/resample.cl'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir64-unknown-unknown"

define cc76 void @PolyEvalOcl(float addrspace(1)* nocapture %coeffs, i32 %order, float addrspace(1)* nocapture %input, float addrspace(1)* nocapture %result) nounwind {
  %1 = tail call cc75 i64 @_Z13get_global_idj(i32 0) nounwind readnone
  %2 = and i64 %1, 4294967295
  %3 = getelementptr inbounds float addrspace(1)* %input, i64 %2
  %4 = load float addrspace(1)* %3, align 4, !tbaa !16
  br label %5

; <label>:5                                       ; preds = %0, %5
  %i.010 = phi i32 [ 0, %0 ], [ %13, %5 ]
  %out.09 = phi float [ 0.000000e+00, %0 ], [ %12, %5 ]
  %6 = zext i32 %i.010 to i64
  %7 = getelementptr inbounds float addrspace(1)* %coeffs, i64 %6
  %8 = load float addrspace(1)* %7, align 4, !tbaa !16
  %9 = uitofp i32 %i.010 to float
  %10 = tail call cc75 float @_Z3powff(float %4, float %9) nounwind readnone
  %11 = fmul float %8, %10
  %12 = fadd float %out.09, %11
  %13 = add i32 %i.010, 1
  %14 = icmp ugt i32 %13, %order
  br i1 %14, label %15, label %5

; <label>:15                                      ; preds = %5
  %16 = getelementptr inbounds float addrspace(1)* %result, i64 %2
  store float %12, float addrspace(1)* %16, align 4, !tbaa !16
  ret void
}

declare cc75 i64 @_Z13get_global_idj(i32) nounwind readnone

declare cc75 float @_Z3powff(float, float) nounwind readnone

define cc76 void @MatrixMultiplier(float addrspace(1)* nocapture %matrixA, float addrspace(1)* nocapture %matrixB, float addrspace(1)* nocapture %matrixC, i32 %widthA, i32 %widthB) nounwind {
  %1 = tail call cc75 i64 @_Z13get_global_idj(i32 0) nounwind readnone
  %2 = trunc i64 %1 to i32
  %3 = tail call cc75 i64 @_Z13get_global_idj(i32 1) nounwind readnone
  %4 = trunc i64 %3 to i32
  %5 = icmp eq i32 %widthA, 0
  br i1 %5, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %0
  %6 = mul i32 %2, %widthA
  br label %7

; <label>:7                                       ; preds = %.lr.ph, %7
  %inner.014 = phi i32 [ 0, %.lr.ph ], [ %19, %7 ]
  %result.013 = phi float [ 0.000000e+00, %.lr.ph ], [ %18, %7 ]
  %8 = add i32 %inner.014, %6
  %9 = zext i32 %8 to i64
  %10 = getelementptr inbounds float addrspace(1)* %matrixA, i64 %9
  %11 = load float addrspace(1)* %10, align 4, !tbaa !16
  %12 = mul i32 %inner.014, %widthB
  %13 = add i32 %12, %4
  %14 = zext i32 %13 to i64
  %15 = getelementptr inbounds float addrspace(1)* %matrixB, i64 %14
  %16 = load float addrspace(1)* %15, align 4, !tbaa !16
  %17 = fmul float %11, %16
  %18 = fadd float %result.013, %17
  %19 = add i32 %inner.014, 1
  %20 = icmp ult i32 %19, %widthA
  br i1 %20, label %7, label %._crit_edge

._crit_edge:                                      ; preds = %7, %0
  %result.0.lcssa = phi float [ 0.000000e+00, %0 ], [ %18, %7 ]
  %21 = mul i32 %2, %widthB
  %22 = add i32 %21, %4
  %23 = zext i32 %22 to i64
  %24 = getelementptr inbounds float addrspace(1)* %matrixC, i64 %23
  store float %result.0.lcssa, float addrspace(1)* %24, align 4, !tbaa !16
  ret void
}

!opencl.kernels = !{!0, !7}
!opencl.enable.FP_CONTRACT = !{}
!opencl.spir.version = !{!14}
!opencl.ocl.version = !{!14}
!opencl.used.extensions = !{!15}
!opencl.used.optional.core.features = !{!15}
!opencl.compiler.options = !{!15}

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
!14 = metadata !{i32 1, i32 2}
!15 = metadata !{}
!16 = metadata !{metadata !"float", metadata !17}
!17 = metadata !{metadata !"omnipotent char", metadata !18}
!18 = metadata !{metadata !"Simple C/C++ TBAA"}
