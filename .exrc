let s:cpo_save=&cpo
set cpo&vim
inoremap <Down> <Nop>
inoremap <Up> <Nop>
inoremap <Right> <Nop>
inoremap <Left> <Nop>
nnoremap  h
vnoremap 	 I	
nnoremap 	 :bnext
nnoremap <NL> j
nnoremap  k
nnoremap  l
nnoremap  c I/*A*/
vnoremap  r "hy:%s/h//g<Left><Left>
nnoremap  	 :buffers
nnoremap  d :bd!
nnoremap  e :Ex
nnoremap  f :find ./**/*
nnoremap  ~ :e ~/.config/nvim/init.lua
nnoremap  ` :so ~/.config/nvim/init.lua
nnoremap  W :wa
nnoremap  w :w
nnoremap  r :make -B -j
omap <silent> % <Plug>(MatchitOperationForward)
xmap <silent> % <Plug>(MatchitVisualForward)
nmap <silent> % <Plug>(MatchitNormalForward)
vnoremap J :m '>+1gv=gv
vnoremap K :m '>-2gv=gv
nnoremap Y y$
omap <silent> [% <Plug>(MatchitOperationMultiBackward)
xmap <silent> [% <Plug>(MatchitVisualMultiBackward)
nmap <silent> [% <Plug>(MatchitNormalMultiBackward)
omap <silent> ]% <Plug>(MatchitOperationMultiForward)
xmap <silent> ]% <Plug>(MatchitVisualMultiForward)
nmap <silent> ]% <Plug>(MatchitNormalMultiForward)
xmap a% <Plug>(MatchitVisualTextObject)
xmap gx <Plug>NetrwBrowseXVis
nmap gx <Plug>NetrwBrowseX
omap <silent> g% <Plug>(MatchitOperationBackward)
xmap <silent> g% <Plug>(MatchitVisualBackward)
nmap <silent> g% <Plug>(MatchitNormalBackward)
xnoremap <silent> <Plug>NetrwBrowseXVis :call netrw#BrowseXVis()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#BrowseX(netrw#GX(),netrw#CheckIfRemote(netrw#GX()))
xmap <silent> <Plug>(MatchitVisualTextObject) <Plug>(MatchitVisualMultiBackward)o<Plug>(MatchitVisualMultiForward)
onoremap <silent> <Plug>(MatchitOperationMultiForward) :call matchit#MultiMatch("W",  "o")
onoremap <silent> <Plug>(MatchitOperationMultiBackward) :call matchit#MultiMatch("bW", "o")
xnoremap <silent> <Plug>(MatchitVisualMultiForward) :call matchit#MultiMatch("W",  "n")m'gv``
xnoremap <silent> <Plug>(MatchitVisualMultiBackward) :call matchit#MultiMatch("bW", "n")m'gv``
nnoremap <silent> <Plug>(MatchitNormalMultiForward) :call matchit#MultiMatch("W",  "n")
nnoremap <silent> <Plug>(MatchitNormalMultiBackward) :call matchit#MultiMatch("bW", "n")
onoremap <silent> <Plug>(MatchitOperationBackward) :call matchit#Match_wrapper('',0,'o')
onoremap <silent> <Plug>(MatchitOperationForward) :call matchit#Match_wrapper('',1,'o')
xnoremap <silent> <Plug>(MatchitVisualBackward) :call matchit#Match_wrapper('',0,'v')m'gv``
xnoremap <silent> <Plug>(MatchitVisualForward) :call matchit#Match_wrapper('',1,'v'):if col("''") != col("$") | exe ":normal! m'" | endifgv``
nnoremap <silent> <Plug>(MatchitNormalBackward) :call matchit#Match_wrapper('',0,'n')
nnoremap <silent> <Plug>(MatchitNormalForward) :call matchit#Match_wrapper('',1,'n')
nnoremap <S-Tab> :bprev
nnoremap <Down> <Nop>
nnoremap <Up> <Nop>
nnoremap <Right> <Nop>
nnoremap <Left> <Nop>
vnoremap <Down> <Nop>
vnoremap <Up> <Nop>
vnoremap <Right> <Nop>
vnoremap <Left> <Nop>
inoremap  u
inoremap  u
inoremap ,. ->
inoremap <C-n <C-x-C-i>
inoremap <C-p <C-x-C-i>
inoremap `sec //***************************************************************************//	 SECTION:***************************************************************************//<Up>	
inoremap `struct typedef struct {@} @;kk0/@
inoremap `while while (@) {@}kk0/@
inoremap `fork for (int k = 0; k < @; k++) {@}kk0/@
inoremap `forj for (int j = 0; j < @; j++) {@}kk0/@
inoremap `fori for (int i = 0; i < @; i++) {@}kk0/@
inoremap `fun @ @ (@) {@}kk/@
inoremap `elif else if (@) {@}kk0/@
inoremap `if if (@) {@}kk0/@
inoremap `for for (@;@;@) {}kk0/@
inoremap `switch switch (@) {case: @;break;}kkk0/@
let &cpo=s:cpo_save
unlet s:cpo_save
set completeopt=menu,menuone,longest,preview
set guicursor=
set helplang=en
set ignorecase
set isfname=#,$,%,+,,,-,.,/,48-57,=,@,_,~,@-@
set runtimepath=~/.config/nvim,/etc/xdg/nvim,~/.local/share/nvim/site,~/.local/share/flatpak/exports/share/nvim/site,/var/lib/flatpak/exports/share/nvim/site,/usr/local/share/nvim/site,/usr/share/nvim/site,/usr/share/nvim/runtime,/usr/share/nvim/runtime/pack/dist/opt/matchit,/usr/lib/nvim,/usr/share/nvim/site/after,/usr/local/share/nvim/site/after,/var/lib/flatpak/exports/share/nvim/site/after,~/.local/share/flatpak/exports/share/nvim/site/after,~/.local/share/nvim/site/after,/etc/xdg/nvim/after,~/.config/nvim/after
set scrolloff=200
set smartindent
set noswapfile
set termguicolors
set undodir=~/.vim/undodir
set window=45
" vim: set ft=vim :
