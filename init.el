(require 'compile)

(setq init-script-path (convert-standard-filename (file-name-directory load-file-name)))
(setq project-root-path init-script-path)
(setq execution-root-path (concat project-root-path "builds"))
(setq build-output-path (concat project-root-path "builds"))
(setq source-path (concat project-root-path "source"))

(setq executable-path (concat build-output-path "\\" "polyroot_test.exe"))
(setq build-script-path (concat project-root-path "build.bat"))
(setq build-command (concat build-script-path " " source-path " " build-output-path))
(setq run-command (concat "cd " execution-root-path " && " executable-path "&"))

;; saves everything and recompiles
(global-set-key (kbd "<f9>")
                (lambda ()
                  (interactive)
                  (setq compile-command build-command)
                  (setq compilation-finish-functions nil)
                  (save-some-buffers 1) ; save all buffers with changes
                  (recompile)))

;; saves, rebuilds and executes the program
(global-set-key (kbd "<f10>")
		(lambda ()
		  (interactive)
          (setq compile-command build-command)
          (save-some-buffers 1) ; save all buffers with changes
          (setq compilation-finish-functions '((lambda (a b)
                                                 (interactive)
                                                 (shell-command run-command))))
          (recompile)))

;; executes the program
(global-set-key (kbd "<f5>")
		(lambda ()
		  (interactive)
		  (shell-command run-command)))
